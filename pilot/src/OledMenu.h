#include <list>
#include <string>


#ifndef OLEDMENU_H
#define OLEDMENU_H


using namespace std;

namespace OledMenu
{
    class MenuItem
    {

    private:
        int int_data;
        bool bool_data;
        string string_data;

        /* data */
    public:
        int id;
        int idf;
        bool folder;
        string name;
        MenuItem(int id, int idf, string name);
        MenuItem(int id, int idf, string name);

    };
    
    MenuItem::MenuItem(int id, int idf, string name)
    {
        this->folder=true;
        this.id = id;
        this->idf= idf;
        this->name = name;
    }
    
    MenuItem::MenuItem(int id, int idf, string name)
    {
       MenuItem::MenuItem(id,idf,name);
       
    }

    {
        seting=true;
        bool_data = data;
    }
    
    
    
    

}