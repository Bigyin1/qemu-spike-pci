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

      qemu-initramfs = pkgs.makeInitrd {
        name = "qemu-initramfs";

        contents = [
          {
            object = "${pkgs.busybox}/bin/busybox";
            symlink = "/busybox";
          }
          {
            object = "${./pcidev/init.sh}";
            symlink = "/init";
          }
        ];
      };

    in
    {
      packages.${system} = { inherit qemu-initramfs; };

      devShells.${system} = {

        default = pkgs.mkShell {
          name = "default";

          inputsFrom = [
            spike-lib
            pkgs.qemu
          ];

          nativeBuildInputs = [
            qemu-initramfs
            pkgs.linux
            qemuSrc
            spike-lib
            pkgs.coreboot-toolchain.riscv
            pkgs.llvmPackages_21.clang-tools
            # pkgs.dtc
          ];

          shellHook = ''

            QEMU_SRC=qemu-src

            if [ ! -d $QEMU_SRC ]; then
              cp -r ${qemuSrc} $QEMU_SRC
              chmod -R +w $QEMU_SRC

              cat pcidev/patches/meson.patch >> $QEMU_SRC/hw/misc/meson.build
              cat pcidev/patches/kconfig.patch >> $QEMU_SRC/hw/misc/Kconfig

              ln pcidev/gpu.c $QEMU_SRC/hw/misc/gpu.c
            fi

            echo "cd $QEMU_SRC && ./configure --target-list=x86_64-softmmu --extra-cflags=-I${spike-lib}/include --extra-ldflags=-L${pkgs.spike}/lib -lfesvr -lriscv -lsoftfloat -ldisasm -L${spike-lib}/lib -lstdc++ -lspikeb" > configure_qemu.sh
            chmod +x ./configure_qemu.sh

            echo "cd $QEMU_SRC && make -j16" > build_qemu.sh
            chmod +x ./build_qemu.sh

            echo "$QEMU_SRC/build/qemu-system-x86_64 -enable-kvm -kernel ${pkgs.linux}/bzImage -initrd ${qemu-initramfs}/initrd.gz -chardev stdio,id=char0 -serial chardev:char0 -append 'quiet console=ttyS0,115200' -display none -m 256  -nodefaults" > run_qemu.sh
            chmod +x ./run_qemu.sh
          '';

        };
      };
    };
}
