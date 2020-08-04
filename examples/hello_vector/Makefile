all: save_app load_app


save_app:
	@echo "Building save_app.."
	@if [ ! -z "${GOOGLE_ROOT}" ]; then \
		ln -fs ../../vector.pb.h ; \
		if [ "$(TOOLCHAIN)" = "Clang" ]; then \
			clang++ -stdlib=libc++ -lc++abi -lsupc++ save_msg.cpp -o save_app -L../.. -lxcmessages -L${GOOGLE_ROOT}/lib -lprotobuf ; \
		else \
			g++ save_msg.cpp -o save_app -L../.. -lxcmessages -L${GOOGLE_ROOT}/lib -lprotobuf ; \
		fi; \
	else \
		ln -f ../../libxcmessages.a libxcmessages.a ; \
		ln -f ../../vector.pb.h vector.pb.h ; \
		c++ save_msg.cpp -o save_app libxcmessages.a -lprotobuf ; \
	fi;


load_app:
	@echo "Building load_app.."
	@if [ ! -z "${GOOGLE_ROOT}" ]; then \
		ln -fs ../../vector.pb.h ; \
		if [ "$(TOOLCHAIN)" = "Clang" ]; then \
			clang++ -stdlib=libc++ -lc++abi -lsupc++ load_msg.cpp -o load_app -L../.. -lxcmessages -L${GOOGLE_ROOT}/lib -lprotobuf ; \
		else \
			g++ save_msg.cpp -o save_app -L../.. -lxcmessages -L${GOOGLE_ROOT}/lib -lprotobuf ; \
		fi; \
	else \
		ln -f ../../libxcmessages.a libxcmessages.a ; \
		ln -f ../../vector.pb.h vector.pb.h ; \
		c++ load_msg.cpp -o load_app libxcmessages.a -lprotobuf ; \
	fi;


clobber:
	@echo "Clobbering.."
	@rm -f save_app load_app
	@rm -f vector.pb.h libxcmessages.a
