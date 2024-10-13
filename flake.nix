{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    infinitime.url = "github:shymega/InfiniTime?ref=shymega-feat/nix-flake&submodules=1";
    # infinitime.url = "github:InfiniTimeOrg/InfiniTime";
    flake-compat.url = "https://flakehub.com/f/edolstra/flake-compat/1.tar.gz";
  };

  outputs = { self, ... }@inputs:
    let
      forAllSystems = function:
        inputs.nixpkgs.lib.genAttrs [
          "x86_64-linux"
          "aarch64-linux"
        ]
          (system: function (import inputs.nixpkgs {
            inherit system;
            config.allowUnfree = true;
          }));
    in
    {
      packages = forAllSystems (pkgs:
        let
          infinitime = inputs.infinitime.packages.${pkgs.system}.default;
        in
        with pkgs; {
          default = stdenv.mkDerivation rec {
            name = "infinisim";
            srcs = [
              (fetchFromGitHub {
                name = "InfiniTime";
                owner = "shymega";
                repo = "InfiniTime";
                rev = "refs/heads/shymega-feat/nix-flake";
                fetchSubmodules = true;
                leaveDotGit = true;
                sha256 = "sha256-3x4FubKg8xXJf4t1QjSRIE8FKh89/0yT67bStH+a3HQ=";
              })
              (lib.cleanSource ./.)
            ];
            sourceRoot = ".";

            nativeBuildInputs = with pkgs; [
              SDL2
              libpng
              patch
              zlib
            ] ++ infinitime.nativeBuildInputs;

            buildInputs = with pkgs; [
              cmake
            ] ++ infinitime.buildInputs;

            preConfigure = ''
              patchShebangs $sourceRoot/InfiniTime/src/displayapp/fonts/generate.py \
                  $sourceRoot/InfiniTime/tools/mcuboot/imgtool.py \
                  $sourceRoot/source/img/convert_bmp_to_header.py
            '';

            cmakeFlags = [
              "-DInfiniTime_DIR=${sourceRoot}/InfiniTime"
            ];
          };
        });

      devShells = forAllSystems (pkgs:
        {
          default =
            pkgs.mkShell {
              packages = [ pkgs.ninja ] ++ self.packages.${pkgs.system}.default.nativeBuildInputs;
            };
        });
    };
}
      
