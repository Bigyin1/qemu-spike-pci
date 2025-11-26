{
  description = "Qemu PCI device";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-25.05";
  };

  outputs =
    { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};

      spike-lib = pkgs.stdenv.mkDerivation {
        name = "spike-lib";

        src = ./spike;
        nativeBuildInputs = [
          pkgs.cmake
          pkgs.spike
        ];
      };

      qemuSrc = pkgs.stdenv.mkDerivation {
        name = "qemu-source";
        src = pkgs.qemu.src;

        unpackPhase = ''
          mkdir -p $out
          tar -xf $src -C $out --strip-components=1
        '';

        dontConfigure = true;
        dontBuild = true;
        dontFixup = true;
        dontInstall = true;
      };

      customQemu = pkgs.qemu.overrideAttrs (old: {

        devSrc = ./pcidev;
        devPatches = ./pcidev/patches;

        prePatch = (old.prePatch or "") + ''
          cp ${devSrc/gpu.c} hw/misc/gpu.c
        '';

        patches = (old.patches or []) ++ [
            devPatches/kconfig.patch
            devPatches/meson.patch
          ];
      });

      pcidev = pkgs.stdenv.mkDerivation {
        name = "pcidev";

        src = ./pcidev;
        nativeBuildInputs = [
          qemuSrc
          spike-lib
          # pkgs.meson
        ];

        dontUnpack = true;
        dontConfigure = true;
        dontBuild = true;
        dontFixup = true;
        dontInstall = true;
      };

    in
    {
      packages.${system} = { inherit pcidev qemuSrc customQemu; };

      devShells.${system} = {

        default = pkgs.mkShell {
          name = "default";

          inputsFrom = [
            spike-lib
            qemuSrc
            pcidev
            pkgs.qemu
          ];
        
          nativeBuildInputs = [
            spike-lib
            qemuSrc
            # pcidev
            pkgs.coreboot-toolchain.riscv
            pkgs.llvmPackages_21.clang-tools
            pkgs.dtc
          ];
        };
      };

      
    };
}
