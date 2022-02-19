# kbdctl

I bought a Logitech G Pro TKL RGB keyboard a few weeks ago, and now I'm sick and tired of the `Logitech G HUB` bloatware I need to install just to alter the lightshow. This is why I'm writing this barebones cli controller, to finally take care of business.

## ToDo

- [X] Locate the keyboard and get a handle to it
- [ ] Apply static color
- [ ] Apply built in animation
- [ ] Apply custom key lighting

## Usage

The controller will only work with the G-Pro, as protocols are not open-source and I'm neither having other keyboards within reach nor the time and dedication to work their protocols out. This is more of a hobby/fun project. To use the software, you only need `hidapi`. The makefile specifies dependencies in homebrews directory (`/opt/homebrew`), if you got your dependencies from somewhere else, alter the makefile.

After making sure the dependency is installed, just run `make`, or `make clean && make` after changes.