#!/usr/bin/env python3
"""Dispatch a build backend's declared command without knowing its toolchain.

Requires Python >= 3.9 (validated with 3.14.6). Toolchain versions and paths
are owned by the selected backend and the external environment.
"""

import argparse
import json
from pathlib import Path
import subprocess
import sys


def select_backend(requested):
    if requested is not None:
        return requested.resolve()
    # Prefer the invoking project; fall back to the project containing this script.
    roots = (Path.cwd(), *Path.cwd().parents, *Path(__file__).resolve().parents)
    build_root = next((root / "build" for root in roots
                       if (root / "build").is_dir()
                       and ((root / "build/build.json").is_file()
                            or any((root / "build").glob("*/build.json")))), None)
    if build_root is None:
        raise ValueError("No build backend found. Run from a project directory or specify --backend PATH")
    if (build_root / "build.json").is_file():
        return build_root.resolve()
    candidates = sorted(path for path in build_root.iterdir()
                        if path.is_dir() and (path / "build.json").is_file())
    if len(candidates) == 1:
        return candidates[0].resolve()
    choices = ", ".join(path.name for path in candidates) or "none"
    raise ValueError(f"Specify --backend; available build backends: {choices}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("command", nargs="?", default="build")
    parser.add_argument("--backend", type=Path, help="Backend directory; otherwise discover build/*/build.json")
    args, forwarded = parser.parse_known_args()
    if forwarded[:1] == ["--"]:
        forwarded = forwarded[1:]
    try:
        backend = select_backend(args.backend)
        manifest = json.loads((backend / "build.json").read_text(encoding="utf-8"))
        if not isinstance(manifest, dict) or manifest.get("schema_version") != 1:
            raise ValueError("Unsupported backend manifest version")
        commands = manifest.get("commands", {})
        if not isinstance(commands, dict):
            raise ValueError("Backend commands must be a mapping")
        command = commands.get(args.command)
        if not isinstance(command, list) or not command or not all(isinstance(item, str) for item in command):
            raise ValueError(f"Backend does not provide command: {args.command}")
        invocation = [item.format(python=sys.executable, backend=str(backend)) for item in command]
        print(f"[build] Command: {args.command}; backend: {backend}", flush=True)
        result = subprocess.run(invocation + forwarded, cwd=backend).returncode
        print(f"[build] {args.command}: {'SUCCESS' if result == 0 else 'FAILED'} (exit code {result})", flush=True)
        return result
    except (OSError, ValueError, KeyError) as error:
        print(f"Build dispatch error: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
