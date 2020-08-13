all: xco2csv


xco2csv:
	@echo "Building xco2csv.."
	@if [ ! -z "${GOOGLE_ROOT}" ]; then \
		ln -fs ../../vector.pb.h ; \
		if [ "$(TOOLCHAIN)" = "Clang" ]; then \
			clang++ -stdlib=libc++ -lc++abi -lsupc++ -lc++fs xco2csv.cpp -o xco2csv -L../.. -lxcmessages -L${GOOGLE_ROOT}/lib -lprotobuf ; \
		else \
			g++ xco2csv.cpp -o xco2csv -L../.. -lxcmessages -L${GOOGLE_ROOT}/lib -lprotobuf ; \
		fi; \
	else \
		ln -f ../../libxcmessages.a libxcmessages.a ; \
		ln -f ../../vector.pb.h vector.pb.h ; \
		c++ xco2csv.cpp -o xco2csv libxcmessages.a -lprotobuf ; \
	fi;


clobber:
	@echo "Clobbering.."
	@rm -f xco2csv
	@rm -f vector.pb.h libxcmessages.a
