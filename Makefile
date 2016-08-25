BASE_DIR=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

BUILD_TYPE=debug
BUILD_INFO=Debug
ifdef RELEASE_DBG_INFO
	BUILD_TYPE=release_dbg_info
	BUILD_INFO=RelWithDebInfo
	export RELEASE_DBG_INFO=1
endif
ifdef RELEASE
	BUILD_TYPE=release
	BUILD_INFO=Release
	export RELEASE=1
endif

ifdef PREFIX
	CMAKE_PREFIX="-DCMAKE_INSTALL_PREFIX=$(shell readlink -f $(PREFIX))"
endif

.PHONY: all build distclean clean release release_dbg_info debug install verifybuildtype
.SILENT:

all: release

build: build/Makefile
	$(MAKE) -C build/

build/Makefile: src/CMakeLists.txt
	@mkdir -p "build"
	@cd "build" && \
		cmake \
		-DCMAKE_BUILD_TYPE=$(BUILD_INFO) \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
		"$(BASE_DIR)/src" \
		 $(CMAKE_PREFIX)

verifybuildtype: build/Makefile
	if ! grep -q "^CMAKE_BUILD_TYPE:STRING=$(BUILD_TYPE)$$" build/CMakeCache.txt > /dev/null; \
		then cd build;cmake -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(BASE_DIR)/src; \
		fi

debug:
	$(MAKE) verifybuildtype DEBUG=1
	$(MAKE) build DEBUG=1

release:
	$(MAKE) verifybuildtype RELEASE=1
	$(MAKE) build RELEASE=1

release_dbg_info:
	$(MAKE) verifybuildtype RELEASE_DBG_INFO=1
	$(MAKE) build RELEASE_DBG_INFO=1

install: build
	$(MAKE) -C build install

uninstall: build/install_manifest.txt
	$(MAKE) -C build uninstall

clean:
	$(MAKE) -C "build" clean

distclean:
	@rm -rf build
