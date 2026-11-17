#!/usr/bin/env python3
"""Build then flash with J-Link Commander; --bin flashes an existing image.

Requires Python >= 3.9 and SEGGER J-Link Software and Documentation Pack.
J-Link Commander V9.94 is the local validation baseline.
Project settings belong to the backend's build.json, not this shared script.
"""

import argparse
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile

from project_build import select_backend


def chip_model(header):
    text = header.read_text(encoding="utf-8")
    match = re.search(r'^\s*#\s*define\s+PROJECT_CHIP_MODEL\s+"([A-Za-z0-9_.-]+)"', text, re.M)
    if match is None:
        raise ValueError(f"Define PROJECT_CHIP_MODEL as a device name string in {header}")
    return match[1]


def version_values(header):
    text = header.read_text(encoding="utf-8")
    name = re.search(r'^\s*#\s*define\s+PROJECT_NAME\s+"([A-Za-z0-9_-]+)"', text, re.M)
    parts = [re.search(r'^\s*#\s*define\s+PROJECT_VERSION_' + part + r'\s+(?:\(\s*([0-9]+)\s*\)|([0-9]+)\b)',
                       text, re.M) for part in ("MAJOR", "MINOR", "PATCH")]
    if name is None or any(part is None for part in parts):
        raise ValueError(f"Invalid project name or version in {header}")
    return {"project": name[1].upper(), "version": ".".join(part[1] or part[2] for part in parts)}


def find_jlink(requested, configured, dry_run):
    candidate = requested or os.environ.get("JLINK") or configured
    path = Path(candidate).expanduser()
    if path.is_file():
        return str(path.resolve())
    found = shutil.which(candidate)
    if found:
        return found
    # The official Windows installer registers its location; no local path is embedded.
    if os.name == "nt" and not (requested or os.environ.get("JLINK")) and candidate.lower() in ("jlink", "jlink.exe"):
        import winreg
        for hive in (winreg.HKEY_CURRENT_USER, winreg.HKEY_LOCAL_MACHINE):
            for view in (winreg.KEY_WOW64_64KEY, winreg.KEY_WOW64_32KEY):
                try:
                    with winreg.OpenKey(hive, r"SOFTWARE\SEGGER\J-Link", 0, winreg.KEY_READ | view) as key:
                        directory = winreg.QueryValueEx(key, "InstallPath")[0]
                    executable = Path(os.path.expandvars(directory)) / "JLink.exe"
                    if executable.is_file():
                        return str(executable.resolve())
                except OSError:
                    continue
    if dry_run:
        return candidate
    raise ValueError("J-Link Commander not found. Set JLINK, add JLink to PATH, or use --jlink PATH")


def commander_script(image, address, run_after):
    filename = image.as_posix()
    if any(character in filename for character in ('"', '\r', '\n')):
        raise ValueError("Firmware paths cannot contain quotes or line breaks")
    if image.suffix.lower() == ".bin":
        load = f'loadfile "{filename}" 0x{address:08X}'
    else:
        load = f'loadfile "{filename}"'
    # LoadFile programs and verifies the image. Do not issue a full-chip erase.
    commands = ["r", "h", load, "r"]
    if run_after:
        commands.append("g")
    else:
        commands.append("h")
    commands.append("exit")
    return "\n".join(commands) + "\n"


def main():
    sys.stdout.reconfigure(encoding="utf-8")
    sys.stderr.reconfigure(encoding="utf-8")
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--backend", type=Path, help="Build backend directory")
    parser.add_argument("--bin", action="store_true", help="Skip building and flash the existing BIN image")
    parser.add_argument("--file", type=Path, help="BIN, HEX or ELF image; default follows project version")
    parser.add_argument("--jlink", help="J-Link Commander executable; overrides JLINK and PATH")
    parser.add_argument("--device", help="J-Link device name")
    parser.add_argument("--interface", type=str.upper, choices=("SWD", "JTAG"))
    parser.add_argument("--speed", type=int, help="Debug interface speed in kHz")
    parser.add_argument("--address", type=lambda value: int(value, 0), help="BIN load address, e.g. 0x08000000")
    parser.add_argument("--serial", help="USB J-Link serial number or nickname")
    parser.add_argument("--no-run", action="store_true", help="Leave the MCU halted after programming")
    parser.add_argument("--dry-run", action="store_true", help="Print commands without starting J-Link")
    parser.add_argument("--timeout", type=float, default=120, help="J-Link timeout in seconds (default: 120)")
    args = parser.parse_args()
    try:
        backend = select_backend(args.backend)
        manifest = json.loads((backend / "build.json").read_text(encoding="utf-8"))
        if manifest.get("schema_version") != 1:
            raise ValueError("Unsupported backend manifest version")
        settings = manifest.get("flash")
        if not isinstance(settings, dict):
            raise ValueError(f"No flash settings in {backend / 'build.json'}")
        if not args.bin:
            build_command = [sys.executable, "-B", str(Path(__file__).with_name("project_build.py")),
                             "build", "--backend", str(backend)]
            if args.dry_run:
                print(">>> DRY RUN: Build command (not executed):", flush=True)
                print(subprocess.list2cmdline(build_command), flush=True)
            else:
                print(">>> Building firmware before flashing...", flush=True)
                build_result = subprocess.run(build_command, cwd=backend, stdin=subprocess.DEVNULL)
                if build_result.returncode != 0:
                    print(f"[flash] Build failed; flashing skipped (exit code {build_result.returncode}).", flush=True)
                    return build_result.returncode
        if args.file is not None:
            image = args.file.expanduser().resolve()
        else:
            values = version_values((backend / settings["version_header"]).resolve())
            image = (backend / settings["image"].format(**values)).resolve()
        if image.suffix.lower() not in (".bin", ".hex", ".elf"):
            raise ValueError("Supported firmware formats: .bin, .hex, .elf")
        image_exists = image.is_file()
        if (not image_exists or image.stat().st_size == 0) and not (
                args.dry_run and not args.bin and args.file is None):
            raise ValueError(f"Firmware missing or empty; build first or use --file: {image}")
        device = args.device or chip_model((backend / settings["project_config"]).resolve())
        interface = args.interface or settings["interface"]
        speed = args.speed if args.speed is not None else settings["speed_khz"]
        address = args.address if args.address is not None else int(str(settings["address"]), 0)
        if interface not in ("SWD", "JTAG") or not isinstance(speed, int) or speed <= 0:
            raise ValueError("Use SWD/JTAG and a positive interface speed in kHz")
        if not device or not 0 <= address <= 0xFFFFFFFF or args.timeout <= 0:
            raise ValueError("Device, 32-bit load address and positive timeout are required")
        if image.suffix.lower() == ".bin" and image_exists and address + image.stat().st_size > 0x100000000:
            raise ValueError("BIN image exceeds the 32-bit address space")
        script = commander_script(image, address, not args.no_run)
        tool = find_jlink(args.jlink, settings.get("jlink", "JLink" if os.name == "nt" else "JLinkExe"), args.dry_run)
        command = [tool, "-Device", device, "-If", interface, "-Speed", str(speed),
                   "-AutoConnect", "1", "-ExitOnError", "1", "-NoGui", "1"]
        if args.serial:
            command.extend(["-USB", args.serial])
        print(f">>> Firmware: {image}", flush=True)
        print(f">>> Target: {device}, {interface}, {speed} kHz", flush=True)
        if image.suffix.lower() == ".bin":
            print(f">>> Load address: 0x{address:08X}", flush=True)
        if args.dry_run:
            print(">>> DRY RUN: No probe connection or flash programming.", flush=True)
            print(subprocess.list2cmdline(command + ["-CommandFile", "<temporary-command-file>"]))
            print(script, end="", flush=True)
            return 0
        with tempfile.TemporaryDirectory(prefix="project-flash-") as temporary:
            command_file = Path(temporary) / "flash.jlink"
            command_file.write_text(script, encoding="utf-8")
            result = subprocess.run(command + ["-CommandFile", str(command_file)],
                                    cwd=backend, stdin=subprocess.DEVNULL, timeout=args.timeout)
        print(f"[flash] {'SUCCESS' if result.returncode == 0 else 'FAILED'} (exit code {result.returncode})", flush=True)
        return result.returncode
    except subprocess.TimeoutExpired:
        print("Flash error: J-Link timed out; check probe and target connections.", file=sys.stderr)
        return 1
    except (OSError, ValueError, KeyError, TypeError) as error:
        print(f"Flash error: {error}", file=sys.stderr)
        return 1
    except KeyboardInterrupt:
        print("Flash interrupted.", file=sys.stderr)
        return 130


if __name__ == "__main__":
    raise SystemExit(main())
