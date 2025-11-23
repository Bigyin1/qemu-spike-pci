{
  description = "Parse JSON in C and C++";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-25.05";
  };

  outputs =
    { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};

      spike-lib = pkgs.stdenv.mkDerivation {
        pname = "spike-lib";
        version = "0.1.0";
        src = ./spike;
        nativeBuildInputs = [
          pkgs.spike
          pkgs.cmake
        ];
      };

      # pcidev = pkgs.stdenv.mkDerivation {
      #   pname = "qemu";
      #   version = "0.1.0";
      #   src = ./pcidev;
      #   nativeBuildInputs = [
      #     pkgs.qemu
      #     spike-lib
      #     pkgs.meson
      #   ];
      # };

    in
    {
      packages.${system} = { inherit spike-lib; };

      devShells.${system} = {

        default = pkgs.mkShell {
          name = "default";
          inputsFrom = [
            spike-lib
            # pcidev
          ];

          nativeBuildInputs = [
            pkgs.dtc
            pkgs.llvmPackages_21.clang-tools
            pkgs.coreboot-toolchain.riscv
          ];
        };
      };
    };
}
