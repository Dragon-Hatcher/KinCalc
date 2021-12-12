# ----------------------------
# Makefile Options
# ----------------------------

NAME = KINCALC
DESCRIPTION = "Solve Kinematic Eqs!"
COMPRESSED = NO
ARCHIVED = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)

THIS_DIR := $(firstword $(MAKEFILE_LIST))

run:
	make
	echo $(MAKEFILE_LIST)
	cemu -s $(abspath $(BINDIR))/$(NAME).8xp
	cemu --launch $(NAME)
