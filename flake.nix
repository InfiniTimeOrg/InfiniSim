{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
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
        with pkgs; {
          default = stdenv.mkDerivation {
            name = "infinisim";
            src = pkgs.lib.cleanSource ./.;

            nativeBuildInputs = [
              cmake
              git
              libpng
              nodePackages.lv_font_conv
              patch
              python3
              python3.pkgs.pillow
              SDL2
              zlib
            ];

            postPatch = ''
              # /usr/bin/env is not available in the build sandbox
              substituteInPlace img/convert_bmp_to_header.py --replace "/usr/bin/env python3" "${python3}/bin/python3"
            '';

            cmakeFlags = [
              ''-DBUILD_DFU=1''
              ''-DBUILD_RESOURCES=1''
            ];
          };
        });

      devShells = forAllSystems (pkgs:
        {
          default =
            pkgs.buildFHSUserEnv {
              name = "infinitime-devenv";
              # build tools
              targetPkgs = pkgs: (with pkgs; [
                cmake
                git
                libpng
                nodePackages.lv_font_conv
                patch
                python3
                python3.pkgs.pillow
                SDL2
                zlib
              ]);
            };
        });
    };
}
      
