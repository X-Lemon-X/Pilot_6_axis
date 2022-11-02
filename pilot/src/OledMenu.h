#include <list>
#include <string>


#ifndef OLEDMENU_H
#define OLEDMENU_H


#define OLEDMENU_INT 1
#define OLEDMENU_STRING 2;
#define OLEDMENU_BOOL 3;
#define OLEDMENU_NOOL 4;

using namespace std;

namespace OledMenu
{

    union Data
    {
        int intD;
        char charD[25];
        bool boolD;
    };

    struct DataGroup
    {
        int type;
        union Data veriable;
    };


    class MenuItem
    {

    private:
        int id;
        int idf;
        bool folder;
        string name;
        DataGroup data;

    public:
        DataGroup GetData();
        MenuItem(int id, int idf, string name);
        MenuItem(int id, int idf, string name, DataGroup data);

    };
    
    MenuItem::MenuItem(int id, int idf, string name)
    {
        this->folder=true;
        this->id = id;
        this->idf= idf;
        this->name = name;
    }

    MenuItem::MenuItem(int id, int idf, string name, struct DataGroup dataG)
    {
        this->folder=true;
        this->id = id;
        this->idf= idf;
        this->name = name;
    }

    DataGroup MenuItem::GetData()
    {
        return this->data;
    }


    class Menu
    {
    private:
        
        int size_andIdCont=0;
        list<MenuItem> menuData_list;
    public:
        Menu(/* args */);
        ~Menu();
        int AddItem(int folder_id, string name, int data);
        int AddItem(int folder_id, string name, char *data);
        int AddItem(int folder_id, string name, bool data);
        int AddFolder(int folder_id, string name);
        struct DataGroup GetItem(int itemID);
    };
    
    Menu::Menu(/* args */)
    {
    }
    
    Menu::~Menu()
    {
    }

    int Menu::AddItem(int folder_id, string name, int data)
    {
        struct DataGroup dg;
        dg.type = OLEDMENU_INT;
        dg.veriable.intD = data;
        menuData_list.push_back(MenuItem(size_andIdCont++, folder_id, name, dg));
        return size_andIdCont;
    }

    
    int Menu::AddItem(int folder_id, string name, char *data)
    {
        struct DataGroup dg;
        dg.type = OLEDMENU_INT;
        strcpy(dg.veriable.charD,data);
        this->menuData_list.push_back(MenuItem(size_andIdCont++, folder_id, name, dg));
        return size_andIdCont;
    }

    
    int Menu::AddItem(int folder_id, string name, bool data)
    {
        struct DataGroup dg;
        dg.type = OLEDMENU_BOOL;
        dg.veriable.boolD = data;
        this->menuData_list.push_back(MenuItem(size_andIdCont++, folder_id, name, dg));
        return size_andIdCont;
    }


    int Menu::AddFolder(int folder_id, string name)
    {
        struct DataGroup dg;
        dg.type = OLEDMENU_NOOL;
        dg.veriable.boolD = false;
        this->menuData_list.push_back(MenuItem(size_andIdCont++, folder_id, name, dg));
        return size_andIdCont;
    }


    struct DataGroup Menu::GetItem(int itemID)
    {
        list<MenuItem>::iterator mi = menuData_list.begin();
        advance(mi,itemID);
        MenuItem mid = *mi;
        return mid.GetData();
    }
    
}

#endif