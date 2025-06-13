{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    infinitime.url = "github:InfiniTimeOrg/InfiniTime";
    flake-compat = {
      url = "github:edolstra/flake-compat";
      flake = false;
    };
  };

  outputs =
    { self, ... }@inputs:
    let
      forAllSystems =
        function:
        inputs.nixpkgs.lib.genAttrs
          [
            "x86_64-linux"
            "aarch64-linux"
          ]
          (
            system:
            function (
              import inputs.nixpkgs {
                inherit system;
                config.allowUnfree = true;
              }
            )
          );
    in
    let
      infinitime = inputs.infinitime.packages.x86_64-linux.infinitime;
      lv_img_conv = inputs.infinitime.packages.x86_64-linux.lv_img_conv;
    in
    {
      packages = forAllSystems (
        pkgs: with pkgs; {
          default = stdenv.mkDerivation rec {
            name = "infinisim";
            version = "v1.13.0";
            srcs =
              let
                InfiniTime = (
                  fetchFromGitHub rec {
                    inherit (infinitime.src) owner repo rev;
                    fetchSubmodules = true;
                    name = repo;
                    sha256 = "sha256-W1aKQbvYQ7yzz1mz2+356i7clkMTXoL5li3YXzysnMw=";
                  }
                );
                InfiniSim = (
                  fetchFromGitHub rec {
                    owner = "InfiniTimeOrg"; # CHANGEME.
                    repo = "InfiniSim";
                    rev = "25ec7af440f3a9044b33ff3462a18589dbbe3dd9";
                    sha256 = "sha256-wRttm6NqGTN4ZeBzWY9ySCkedKDu70A1pUPRA87IuTg=";
                  }
                );
              in
              [
                InfiniSim
                InfiniTime
              ];

            sourceRoot = ".";
            postUnpack =
              let
                InfiniTime = (builtins.elemAt srcs 1).name;
                InfiniSim = (builtins.elemAt srcs 0).name;
              in
              ''
                cp -R ${InfiniSim} $sourceRoot/InfiniSim
                cp -R ${InfiniTime} $sourceRoot/InfiniSim/InfiniTime
                chmod -R u+w $sourceRoot/InfiniSim
              '';

            nativeBuildInputs =
              with pkgs;
              [
                SDL2
                libpng
                patch
                zlib
              ]
              ++ infinitime.nativeBuildInputs;

            buildInputs = with pkgs; [ cmake ] ++ infinitime.buildInputs;

            preConfigure =
              let
                InfiniSim = "$sourceRoot/InfiniSim";
                InfiniTime = "${InfiniSim}/InfiniTime";
              in
              ''
                substituteInPlace ${InfiniTime}/src/displayapp/fonts/generate.py --replace "'/usr/bin/env', 'patch'" "'${lib.getExe patch}'"
                substituteInPlace ${InfiniTime}/src/resources/generate-fonts.py --replace "'/usr/bin/env', 'patch'" "'${lib.getExe patch}'"
                patchShebangs ${InfiniTime}/src/displayapp/fonts/generate.py \
                    ${InfiniTime}/tools/mcuboot/imgtool.py
              '';

            cmakeBuildDir = "InfiniSim/build";

            cmakeFlags =
              let
                lvImgConvPath = "${lv_img_conv.outPath}/bin";
              in
              [
                "-DCMAKE_PROGRAM_PATH=${lvImgConvPath}"
                "-DInfiniTime_DIR=../InfiniTime"
              ];

            meta = with lib; {
              maintainers = with maintainers; [ shymega ];
              metaProgram = "infinisim";
            };
          };
        }
      );

      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell {
          packages = [ pkgs.ninja ] ++ self.packages.${pkgs.system}.default.nativeBuildInputs;
        };
      });
    };
}
