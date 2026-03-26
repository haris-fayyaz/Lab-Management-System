CXX = g++
CXXFLAGS = -std=c++11 -Wall -Iinclude
LIBS_GUI = -lglfw -lGL -ldl

# ImGui sources
IMGUI_DIR = imgui
IMGUI_SOURCES = $(IMGUI_DIR)/imgui.cpp \
                $(IMGUI_DIR)/imgui_draw.cpp \
                $(IMGUI_DIR)/imgui_widgets.cpp \
                $(IMGUI_DIR)/imgui_tables.cpp \
                $(IMGUI_DIR)/imgui_impl_glfw.cpp \
                $(IMGUI_DIR)/imgui_impl_opengl3.cpp

IMGUI_OBJECTS = $(IMGUI_SOURCES:.cpp=.o)

# Targets
TARGET_CONSOLE = LabsManagementSystem
TARGET_GUI = LabsManagementSystemGUI

# Source files
SRC_CONSOLE = main.cpp
SRC_GUI = main_gui.cpp

# Default target
all: console

# Console version (original)
console: $(TARGET_CONSOLE)

$(TARGET_CONSOLE): $(SRC_CONSOLE)
	$(CXX) $(CXXFLAGS) -o $(TARGET_CONSOLE) $(SRC_CONSOLE)
	@echo "Console version compiled successfully!"

# GUI version
gui: $(TARGET_GUI)

$(TARGET_GUI): $(IMGUI_OBJECTS) $(SRC_GUI)
	$(CXX) $(CXXFLAGS) -I$(IMGUI_DIR) -o $(TARGET_GUI) $(SRC_GUI) $(IMGUI_OBJECTS) $(LIBS_GUI)
	@echo "GUI version compiled successfully!"

# Compile ImGui sources
$(IMGUI_DIR)/%.o: $(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(IMGUI_DIR) -c $< -o $@

# Clean
clean:
	rm -f $(TARGET_CONSOLE) $(TARGET_GUI)
	rm -f $(IMGUI_OBJECTS)
	rm -rf data/
	@echo "Clean complete!"

# Run targets
run-console: $(TARGET_CONSOLE)
	./$(TARGET_CONSOLE)

run-gui: $(TARGET_GUI)
	./$(TARGET_GUI)

# Install dependencies (Ubuntu/Debian)
install-deps:
	@echo "Installing dependencies..."
	sudo apt-get update
	sudo apt-get install -y build-essential libglfw3-dev libgl1-mesa-dev libglu1-mesa-dev
	@echo "Dependencies installed!"

# Download ImGui
setup-imgui:
	@echo "Downloading ImGui..."
	@if [ ! -d "$(IMGUI_DIR)" ]; then \
		git clone https://github.com/ocornut/imgui.git imgui_temp && \
		mkdir -p $(IMGUI_DIR) && \
		cp imgui_temp/imgui.h $(IMGUI_DIR)/ && \
		cp imgui_temp/imgui.cpp $(IMGUI_DIR)/ && \
		cp imgui_temp/imgui_draw.cpp $(IMGUI_DIR)/ && \
		cp imgui_temp/imgui_widgets.cpp $(IMGUI_DIR)/ && \
		cp imgui_temp/imgui_tables.cpp $(IMGUI_DIR)/ && \
		cp imgui_temp/imgui_internal.h $(IMGUI_DIR)/ && \
		cp imgui_temp/imconfig.h $(IMGUI_DIR)/ && \
		cp imgui_temp/imstb_rectpack.h $(IMGUI_DIR)/ && \
		cp imgui_temp/imstb_textedit.h $(IMGUI_DIR)/ && \
		cp imgui_temp/imstb_truetype.h $(IMGUI_DIR)/ && \
		cp imgui_temp/backends/imgui_impl_glfw.h $(IMGUI_DIR)/ && \
		cp imgui_temp/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/ && \
		cp imgui_temp/backends/imgui_impl_opengl3.h $(IMGUI_DIR)/ && \
		cp imgui_temp/backends/imgui_impl_opengl3.cpp $(IMGUI_DIR)/ && \
		cp imgui_temp/backends/imgui_impl_opengl3_loader.h $(IMGUI_DIR)/ && \
		rm -rf imgui_temp && \
		echo "ImGui setup complete!"; \
	else \
		echo "ImGui directory already exists!"; \
	fi

# Setup everything for GUI
setup-all: install-deps setup-imgui
	@echo "Setup complete! You can now run 'make gui' to compile."

.PHONY: all console gui clean run-console run-gui install-deps setup-imgui setup-all