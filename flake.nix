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

      qemu-pci = pkgs.stdenv.mkDerivation {
        pname = "qemu-pci";
        version = "0.1.0";
        src = ./src;
        nativeBuildInputs = [
          pkgs.llvmPackages_21.clang-tools
          pkgs.qemu.src
          pkgs.spike
          pkgs.cmake
        ];
      };
    in
    {
      packages.${system} = { inherit qemu-pci; };
      devShells.${system}.default = pkgs.mkShell {
        name = "qemu-pci";
        inputsFrom = [
          qemu-pci
        ];
      };
    };
}

