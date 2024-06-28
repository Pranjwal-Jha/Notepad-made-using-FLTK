#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Box.h>
#include <FL/Fl_Output.H>
#include <FL/Fl_Window.h>
#include <FL/Fl_Input.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Sys_Menu_Bar.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Menu_.H>
#include <FL/Fl_Menu.H>
#include <fstream>
#include <iostream>
#include <string>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Menu_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Copy_Surface.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Input.H>
#include "forward_declaration.h"
#pragma warning(disable:4996)
//replace dialogue box
	// replace all
	// replace next
	// replace with
	// replace find
	// replace cancel
	// search 

class EditorWindow: public Fl_Double_Window
{
public:
	EditorWindow(int width, int height, const char* t)
		:Fl_Double_Window(width, height, t)
	{
	}
	~EditorWindow()
	{
	}

	Fl_Window* replace_dlg = nullptr;
	Fl_Input* replace_find = nullptr;
	Fl_Input* replace_with = nullptr;
	Fl_Button* replace_all = nullptr;
	Fl_Return_Button* replace_next =nullptr;
	Fl_Text_Editor* text_editor = nullptr;
	char search[256];
	Fl_Text_Buffer* textbuffer;
	std::string current_filename; // Add this line

	void set_filename(const char* filename) {
		current_filename = filename ? filename : "";
	}
};

int changed = 0;
char filename[256] = "";
Fl_Text_Buffer* textbuffer;  //-> text editor buffer 
int loading = 0;
EditorWindow* w;
void changed_cb(int, int nInserted, int nDeleted, int, const char*, void* v)
{
	if ((nInserted || nDeleted) && !loading) 
		changed = 1;
	EditorWindow* w = (EditorWindow*)v;
	set_title(w);
	if (loading) w->text_editor->show_insert_position();
}
void copy_cb(Fl_Widget*, void* v)
{
	EditorWindow* e = (EditorWindow*)v;
	Fl_Text_Editor::kf_copy(0, e->text_editor);
}
void delete_cb(Fl_Widget*, void* v)
{
	EditorWindow* e = (EditorWindow*)v;
	Fl_Text_Editor::kf_cut(0, e->text_editor);
}
void find2_cb(Fl_Widget* w, void* v)
{
	EditorWindow* e = (EditorWindow*)v;
	if (e->search[0] == '\0')
	{
		find_cb(w, v);
		return;
	}
	int pos = e->text_editor->insert_position();
	int found = textbuffer->search_forward(pos, e->search, &pos);
	if (found)
	{
		textbuffer->select(pos, pos + strlen(e->search));
		e->text_editor->insert_position(pos + strlen(e->search));
		e->text_editor->show_insert_position();
	}
	else fl_alert("no occuerence of \'%s\' found !", e->search);
}
void find_cb(Fl_Widget* w, void* v)
{
	EditorWindow* e = (EditorWindow*)v;
	const char* val;
	val = fl_input("search string :", e->search);
	if (val != NULL)
	{
		strcpy(e->search, val);
		find2_cb(w, v);
	}
}
void new_cb(Fl_Widget*, void* v)
{
	EditorWindow* e = (EditorWindow*)v;
	if (!check_save(e))
		return;
	filename[0] = '\0';
	textbuffer->select(0, textbuffer->length());
	textbuffer->remove_selection();
	changed = 0;
	textbuffer->call_modify_callbacks();
}

void open_cb(Fl_Widget*, void* v)
{
	EditorWindow* e = (EditorWindow*)v;
	if (!check_save(e))
		return;
	char* newfile = fl_file_chooser("Open file ?", "*", filename);
	if (newfile != NULL)
		load_file(newfile, -1);
}
void paste_cb(Fl_Widget*, void* v)
{
	EditorWindow* e = (EditorWindow*)v;
	Fl_Text_Editor::kf_paste(0, e->text_editor);
}
void quit_cb(Fl_Widget*, void* v)
{
	EditorWindow* e = (EditorWindow*)v;
	if (changed && !check_save(e))
		return;
	exit(0);
}
void replace_cb(Fl_Widget*, void* v)
{
	EditorWindow* e = (EditorWindow*)v;
	e->replace_dlg->show();
}


int check_save(void* v)
{
	EditorWindow* window = (EditorWindow*)v;
	if (!changed)
		return 1;
	int r = fl_choice("The current file has not been saved \n"
		"woudll you like to save it now ?",
		"Cancel", "Save", "Discard");
	if (r == 1)
	{
		save_cb(nullptr,window);
		return ~changed;
	}
	return (r == 2) ? 1 : 0;
}

void load_file(char* newfile, int ipos)
{
	loading = 1;
	int insert = (ipos != -1);
	changed = insert;
	if (!insert)
		strcpy(filename, "");
	int r;
	if (!insert)
		r = textbuffer->loadfile(newfile);
	else
		r = textbuffer->insertfile(newfile, ipos);
	if (r)
		fl_alert("error reading from the file \'%s\' : \n%s.", newfile, strerror(errno));
	else
		if (!insert)
			strcpy(filename, newfile);
	loading = 0;
	textbuffer->call_modify_callbacks();
}
//void save_cb()
//{
//	if (filename[0] == '\0')
//	{
//		saveas_cb();
//		return;
//	}
//	else
//		save_file(filename);
//}
//
//void saveas_cb()
//{
//	char* newfile;
//	newfile = fl_file_chooser("Save file as ? :", "*", filename);
//	if (newfile != NULL)
//		save_file(newfile);
//}
//void save_file(char* newfile)
//{
//	if (textbuffer->savefile(newfile))
//		fl_alert("error writing to the file \'%s\' : \n%s.", newfile, strerror(errno));
//	else
//		strcpy(filename, newfile);
//	changed = 0;
//	textbuffer->call_modify_callbacks();
//}
bool save_file(const char* filename, Fl_Text_Buffer* textbuffer) {
	std::ofstream ofs(filename);
	if (!ofs) {
		fl_alert("Error: Cannot open file for writing.");
		return false;
	}
	ofs << textbuffer->text();
	return true;
}
bool save_as(Fl_Text_Buffer* textbuffer) 
{
	Fl_Native_File_Chooser file_chooser;
	file_chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	file_chooser.title("Save As");
	file_chooser.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM);

	switch (file_chooser.show())
	{
	case -1:  // Error
		fl_alert("%s", file_chooser.errmsg());
		return false;
	case 1:   // Cancel
		return false;
	default:  // User picked a file
		return save_file(file_chooser.filename(), textbuffer);
	}
}

void set_title(Fl_Window* w)
{
	char title[256];
	if (filename[0] == '\0')
		strcpy(title, "Untitled");
	else
	{
		char* slash = filename;
		filename[0] = '/';
		//#ifdef WIN32
		//if (slash == NULL)
		//	slash = strcpy(filename, '\\');
		//#endif
		if (slash != NULL)
			strcpy(title, slash + 1);
		else
			strcpy(title, filename);
	}
	if (changed)	
		strcat(title, " (modified)");
	w->label(title);
}
// Save callback function
void save_cb(Fl_Widget*, void* data)
{
	EditorWindow* window = (EditorWindow*)data;
	static const char* filename = "default.txt";  // Replace with logic to get current file name if needed

	if (!save_file(window->current_filename.c_str(), window->textbuffer)) {
		// If save failed, fall back to "Save As"
		//save_as(window->textbuffer);
	}
}

// Save As callback function
void saveas_cb(Fl_Widget*, void* data) 
{
	EditorWindow* window = (EditorWindow*)data;
	save_as(window->textbuffer);
}

//Fl_Menu_Item menuitems[] = {
//		{"&File",0,,0,0,FL_SUBMENU},
//		{"&Open File", FL_CTRL + 'o' , (Fl_Callback*)open_cb},
//		{"&Insert File", FL_CTRL + 'i', (Fl_Callback*)insert_cb,0,FL_MENU_DIVIDER},
//		{"&Save File" , FL_CTRL + 's' , (Fl_Callback*)save_cb},
//		{"&Save File As", FL_CTRL + FL_SHIFT + 's',(Fl_Callback * saveas_cb,0,FL_MENU_DIVIDER},
//		{"&Close View", FL_CTRL + 'w',(Fl_Callback*)close_cb,0},
//		{"&Exit",FL_CTRL + 'q', (Fl_Callback*)quit_cb,0},
//		{0},
//		{"&Edit",0,0,0,FL_SUBMENU},
//		{"&Undo", FL_CTRL + 'z',(Fl_Callback*)undo_cb,0,FL_MENU_DIVIDER},
//		{"&Copy",FL_CTRL + 'c',(Fl_Callback*)copy_cb},
//		{"&Paste",FL_CTRL + 'p',(Fl_Callback*)paste_cb},
//		{"&Delete",0,(Fl_Callback*)delete_cb},
//		{0},
//		{"&Search",0,0,0,FL_SUBMENU},
//		{"&Find", FL_CTRL + 'f', (Fl_Callback*)find_cb},
//		{"&Replace", FL_CTRL + 'r',replace_cb},
//		{0},
//		{0},
//
//};
Fl_Menu_Item menuitems[] = {
	{"&File", 0, 0, 0, FL_SUBMENU},
	{"&Open File", FL_CTRL + 'o', (Fl_Callback*)open_cb},
	//{"&Insert File", FL_CTRL + 'i', (Fl_Callback*)insert_cb, 0, FL_MENU_DIVIDER},
	{"&Save File", FL_CTRL + 's', (Fl_Callback*)save_cb,0},
	{"&Save File As", FL_CTRL + FL_SHIFT + 's', (Fl_Callback*)saveas_cb, 0, FL_MENU_DIVIDER},
	//{"&Close View", FL_CTRL + 'w', (Fl_Callback*)close_cb, 0},
	{"&Exit", FL_CTRL + 'q', (Fl_Callback*)quit_cb, 0},
	{0},
	{"&Edit", 0, 0, 0, FL_SUBMENU},
	//{"&Undo", FL_CTRL + 'z', (Fl_Callback*)undo_cb, 0, FL_MENU_DIVIDER},
	{"&Copy", FL_CTRL + 'c', (Fl_Callback*)copy_cb},
	{"&Paste", FL_CTRL + 'v', (Fl_Callback*)paste_cb},  // 'v' is a common shortcut for paste
	{"&Delete", 0, (Fl_Callback*)delete_cb},
	{0},
	{"&Search", 0, 0, 0, FL_SUBMENU},
	{"&Find", FL_CTRL + 'f', (Fl_Callback*)find_cb},
	{"&Replace", FL_CTRL + 'r', (Fl_Callback*)replace_cb},
	{0},
	{0}
};
//Fl_Menu_Bar* menubar = new Fl_Menu_Bar(0, 0, 640, 30);
//menubar->add(menuitems);
Fl_Button* cancel = new Fl_Button(230, 70, 60, 25,"Cancel");
int main(int argc, char** argv)
{
	
	textbuffer = new Fl_Text_Buffer;
	w = new EditorWindow(640, 400, "window");
    w->text_editor = new Fl_Text_Editor(0, 30, 640, 370);
    w->text_editor->buffer(textbuffer);
    textbuffer->add_modify_callback(changed_cb, w);
    textbuffer->call_modify_callbacks();
    //text_editor->textfont(FL_COURIER);
	Fl_Menu_Bar* menubar = new Fl_Menu_Bar(0, 0, 640, 30);
	menubar->copy(menuitems);
    Fl_Window* replace_dlg = new Fl_Window(300, 105, "Replace");
    Fl_Input* replace_find = new Fl_Input(70, 10, 200, 25, "Find");
    Fl_Input* replace_with = new Fl_Input(70, 40, 200, 25, "Replace");
    Fl_Button* replace_all = new Fl_Button(10, 70, 90, 25, "Replace All");
    Fl_Return_Button* replace_next = new Fl_Return_Button(105, 70, 120, 25 ,"Replace Next");
    w->show(1, argv);
	if (argc > 1)
		load_file(argv[1], -1);
	return Fl::run();
}
