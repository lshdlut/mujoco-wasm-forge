# Supported MuJoCo versions (committed dist/)

This repository can build **any** upstream MuJoCo ref via:

```bash
python3 forge_cli.py build --version <mjver>
```

For convenience (and for CI `verify-dist`), some `dist/<ver>/` outputs are committed to git.

## Versions currently present in `dist/`

- `3.3.7`
- `3.4.0`
- `3.5.0`

## Release tags (convention)

CI treats tags matching `forge-*` as release-like triggers, e.g.:

- `forge-3.4.0-r1`
- `forge-3.5.0-r1`

