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
        int GetId();
        int GetIdFolder();
        bool GetIfFolder();
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

    int MenuItem::GetId()
    {
        return id;
    }

    int MenuItem::GetIdFolder()
    {
        return idf;
    }

    bool MenuItem::GetIfFolder()
    {
        return folder;
    }

    DataGroup MenuItem::GetData()
    {
        return this->data;
    }


    class Folder
    {

    private:
        int idf;
        string name;
    public:
        MenuItem(int id, int idf, string name);
        int GetIdFolder();
        bool GetIfFolder();
    };

    Folder::Folder(int id, int idf, string name)
    {
        this->id = id;
        this->idf= idf;
        this->name = name;
    }

    int Folder::GetId()
    {
        return id;
    }

    int Folder::GetIdFolder()
    {
        return idf;
    }


    DataGroup MenuItem::GetData()

    class Menu
    {
    private:
        
        int size_andIdCont=-1;
        list<Folders> folders_list;
        list<MenuItem> menuData_list;
    public:
        Menu(/* args */);
        ~Menu();
        int AddItem(int folder_id, string name, int data);
        int AddItem(int folder_id, string name, char *data);
        int AddItem(int folder_id, string name, bool data);
        int AddFolder(int folder_id, string name);
        struct DataGroup GetData(int itemID);
        MenuItem GetItem(int itemID);
        void FinishSetup();
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


    struct DataGroup Menu::GetData(int itemID)
    {
        list<MenuItem>::iterator mi = menuData_list.begin();
        advance(mi,itemID);
        MenuItem mid = *mi;
        return mid.GetData();
    }

    MenuItem Menu::GetItem(int itemID)
    {
        list<MenuItem>::iterator mi = menuData_list.begin();
        advance(mi,itemID);
        MenuItem mid = *mi;
        return mid;
    }


    void Menu::FinishSetup(int *foldersIds, int *sizeFolders, int *itemsIds, int *sizeItems, int folderId)
    {
        int sizeF=0;
        int sizeIte=0;
        List<MenuItem>::iterator ite;
        int id=0;

        for (ite = menuData_list.begin(); ite != menuData_list.end(); ite++)
        {
            MenuItem mi = *ite.
            if(mi.GetIfFolder() && mi.GetIdFolder() == folderId)
            {
                *foldersIds[id] = id;
                sizeF++;  
            }
            else if( !mi.GetIfFolder() && mi.GetIdFolder() == folderId)
            {
                *itemsIds[id] = id;
                sizeIte++;  
            } 

            id++;
        }

        *sizeFolders = sizeF;
        *sizeItems = sizeIte;
    }   

}

#endif