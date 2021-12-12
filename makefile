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
	cemu -s $(THIS_DIR)/../bin/$(NAME).8xp
	cemu --launch $(NAME)
