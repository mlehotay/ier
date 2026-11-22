# Minimal Lean installation on Ubuntu in WSL

Install Elan and one Linux Lean toolchain inside Ubuntu in WSL. Use Lean
4.19.0, the version checked against the four examples in this directory. This
setup runs proofs from Bash without VS Code, Mathlib, Conda, or a compiler build.
Lean's distribution includes its standard library and Lake; those arrive with
the toolchain even though these examples do not need a Lake project.

These instructions adapt the [official manual installation guide](https://lean-lang.org/install/manual/)
and [Elan installer options](https://github.com/leanprover/elan/blob/master/elan-init.sh)
to this repository. Run them in the Ubuntu WSL terminal, under your normal Linux
account. They install the Linux tools in your WSL home directory.

## 1. Check space and prerequisites

```sh
df -h "$HOME"
command -v curl git make
```

Allow about 5 GB free for this pinned installation, including download and
unpacking headroom. This is a practical estimate, not an official minimum.
The Linux x86-64 Lean 4.19.0 archive used for repository validation was about
328 MiB, and its extracted toolchain occupied about 1.6 GiB. Additional Lean
versions and future Mathlib dependencies would require additional space.

If any commands are missing, install the prerequisites:

```sh
sudo apt update
sudo apt install curl git make ca-certificates
```

## 2. Install Elan and the tested Lean version

Download the official installer, then run it with the exact toolchain selected:

```sh
curl -fsSL https://elan.lean-lang.org/elan-init.sh -o /tmp/ier-elan-init.sh
sh /tmp/ier-elan-init.sh -y --default-toolchain leanprover/lean4:v4.19.0
source "$HOME/.elan/env"
```

`-y` accepts the installer defaults. The explicit toolchain selects 4.19.0
instead of also installing the latest stable release. Do not run the Elan
installer with `sudo`.

Elan installs under `~/.elan` and updates the shell profile to add its commands
to `PATH`. The `source` command makes them available in the current terminal.
This sets 4.19.0 as your user default; another project's toolchain file or a
directory override can select a different version. See
[Elan toolchain selection](https://lean-lang.org/doc/reference/latest/Build-Tools-and-Distribution/Managing-Toolchains-with-Elan/).

## 3. Verify the installation

```sh
command -v lean
lean --version
elan toolchain list
elan show
```

`command -v lean` should point inside your Linux home directory's `.elan/bin/`
directory, normally `/home/<your-linux-username>/.elan/bin/`.
The version should report Lean 4.19.0, and the toolchain list should include
`leanprover/lean4:v4.19.0`. If Elan was already installed, other versions may
also appear.

If `lean` is unavailable or resolves to a Windows executable, reload the Linux
Elan environment and check again:

```sh
source "$HOME/.elan/env"
hash -r
command -v lean
lean --version
```

Do not install Ubuntu's `elan` package in response to a command-not-found
suggestion after using the official installer; first correct the `PATH`.

## 4. Check the repository examples

Open a terminal at the root of your development checkout, where the repository's
`Makefile` is located, and run:

```sh
make check-lean
```

This checks unity, slack, Vector pickup, and the Epistemic Kettle. It writes
`build/lean/IER-unity.olean` so Vector's import can resolve. The remaining
proofs are checked directly. Successful output reports no axiom dependencies
for the printed theorems and ends without an error.

To select the tested version explicitly even if a different default is active:

```sh
make check-lean LEAN='lean +leanprover/lean4:v4.19.0'
```

## Running from the public checkout

The public export includes the Lean files but not the development repository's
Make target. From its `apps/lean` directory, run:

```sh
lean -o IER-unity.olean IER-unity.lean
lean IER-slack.lean
LEAN_PATH=. lean vector-pickup.lean
lean epistemic-kettle.lean
```

Return to the [Lean README](README.md) for the meaning and limits of the proofs.
