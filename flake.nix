{
  inputs.nixpkgs.url = "github:nixos/nixpkgs/release-21.05";
  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let
          fugioDefaults = {
            version = "1.5.66";
          };
          pkgs = nixpkgs.legacyPackages.${system};
          defaultDeps = with pkgs; [
            gcc
            cmake
            lua5_3
            snappy
            ffmpeg
            fftw
            fftwFloat
            fftwSinglePrec
            fftwLongDouble
            opencv3
            portaudio
            portmidi

            pkg-config
            qt5.qtbase
            qt5.qttools
            qt5.qtwebsockets
            qt5.qtserialport
            qt5.qtquickcontrols2
          ];

          fugio = pkgs.stdenv.mkDerivation (fugioDefaults // {
            name = "fugio";
            
            # unfortuntelly, this does not work.
            # https://github.com/NixOS/nix/issues/4423

            src = ./.;
            # src= {url=./.; submodules=true;};
            #src = (pkgs.nix-gitignore.gitignoreSource "/DOESNOTEXIST" ./.); 
            #src = self;
            
            nativeBuildInputs = [ pkgs.qt5.wrapQtAppsHook ]; 
            buildInputs = defaultDeps; # ++ [libfugio];

            });
        in
        {
          packages = {
            # libfugio = libfugio;
            fugio = fugio;
          };

          defaultPackage = self.packages.${system}.fugio;
          devShell = pkgs.mkShell {
            buildInputs = defaultDeps;
           };
        }
      );
}