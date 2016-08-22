BASE_DIR=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
BUILD_DIR=$(BASE_DIR)/build/


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

.PHONY: all build distclean clean
.SILENT:

all: release

build: $(BUILD_DIR)/Makefile
	$(MAKE) -C $(BUILD_DIR)/

$(BUILD_DIR)/Makefile: src/CMakeLists.txt
	@mkdir -p "$(BUILD_DIR)"
	@cd "$(BUILD_DIR)" && \
		cmake \
		-DCMAKE_BUILD_TYPE=$(BUILD_INFO) \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
		"$(BASE_DIR)/src" \
		 $(CMAKE_PREFIX)

debug:
	$(MAKE) build DEBUG=1

release:
	$(MAKE) build RELEASE=1

install: release
	$(MAKE) -C $(BUILD_DIR) install

clean:
	$(MAKE) -C "$(BUILD_DIR)" clean

distclean:
	@rm -rf $(BUILD_DIR)
