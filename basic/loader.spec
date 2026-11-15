x64:
	load "../bin/loader.x64.o"
		make pic +gofirst
		dfr "resolve" "ror13"
		dfr "resolve_ext" "strings"
		mergelib "../agent/libs/libtcg.x64.zip"
		run "../agent/libs/cp-dfr-defs/dfr_agent.spec"

		push $DLL
			link "my_data"

		load "../bin/basic_pico.x64.o"
			make object
			mergelib "../bin/libdebug.x64.zip"
			mergelib "../bin/libtransport.x64.zip"
			mergelib "../bin/libutils.x64.zip"
			run "../agent/libs/cp-dfr-defs/dfr_agent.spec"
			export
			link "my_pico"

	export
