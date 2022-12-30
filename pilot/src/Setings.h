#include <string>
#include <map>




namespace Setings
{
    union setings_seting_data
    {
        int _int;
        std::string _string;
        float _float;
        bool _bool;
    };

    enum setings_dataType
    {
        _int = 1;
        _float = 2;
        _bool = 3;
        _string = 4;
    };

    struct setings_seting
    {
        union setings_seting_data data;
        enum setings_dataType type;
    };

    class Setings
    {
    private:
    std::map<string, setings_seting> _setings;
    public:
        void AddSeting(string name,setings_data seting);
        void ModifySeting(setings_data seting);
        int AutoConvertSetings(string name, string input_data);
        
        Setings(/* args */);
        ~Setings();
    };


    void Setings::AddSeting(string name, setings_data seting)
    {
    
    }

    int Setings::AutoConvertSetings(string name, string input_data)
    {
        int res = _setings.count(name);
        if(res == 0)
        {
            _setings.insert(name,seting);
        }
        else
        {
            setings_seting ss = _setings[name];
          
        }
    }

    Setings::Setings(/* args */){}
    Setings::~Setings(){}


    int main(int argc, char const *argv[])
    {
       Setings ss;
       string key = "vale";

       ss.AutoConvertSetings(key,"12");
        return 0;
    }
    
}



