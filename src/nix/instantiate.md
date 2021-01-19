R""(

# Examples

* Instantiate the default package from the flake in the current directory:

  ```console
  # nix instantiate
  /nix/store/p5gfr5w0x21pncsf4s76jvvfifrxycfm-nix-2.4pre20210118_dirty.drv
  ```

* Instantiate GNU Hello and Cowsay, printing two derivation paths:

  ```console
  # nix instantiate nixpkgs#hello nixpkgs#cowsay
  /nix/store/6vc4ga99yh2cblq6kll13dlc15c19k92-hello-2.10.drv
  /nix/store/ips662m9x7m4l14ip812ycqw7dk9zsdi-cowsay-3.03+dfsg2.drv
  ```

* Instantiate attribute `iso_minimal.x86_64-linux` from (non-flake) Nix expression
  `release.nix`:

  ```console
  # nix instantiate -f release.nix iso_minimal.x86_64-linux
  /nix/store/04gr7vw3ish8w2y92fdi894rfpkqjz5z-nixos-minimal-21.03pre130979.gfedcba-x86_64-linux.iso.drv
  ```

* Instantiate an expression specified on the command line:

  ```console
  # nix instantiate --impure --expr \
      'with import <nixpkgs> {};
       runCommand "foo" {
         buildInputs = [ hello ];
       }
       "hello > $out"'
  /nix/store/xv21rz83lq2w5vrgh9il54il8y8bcphx-foo.drv
  ```

  Note that `--impure` is needed because we're using `<nixpkgs>`,
  which relies on the `$NIX_PATH` environment variable.

# Description

`nix instantiate` instantiates the specified *installables* and prints their
store paths. Installables that do not resolve to derivations raise an error.

)""
