#include <string>
#include <map>

namespace Setings
{

    enum setings_ERRORS
    {
        ERRORS_OK=0,
        ERRORS_NO_KEY = 1,
        ERROR_INVALID_CONVERSION =2
    };

    union setings_seting_data
    {
        int _int;
        std::string *_string;
        float _float;
        bool _bool;
    };

    enum setings_dataType
    {
        _int = 1,
        _float = 2,
        _bool = 3,
        _string = 4
    };

    typedef struct setings_seting
    {
        union setings_seting_data data;
        enum setings_dataType type;
    };

    class Setings
    {
        private:
            std::map<string, setings_seting> _setings;
        
        
        public:
            
            void AddSeting(std::string name, int seting);
            void AddSeting(std::string name, float seting);
            void AddSeting(std::string name, bool seting);
            void AddSeting(std::string name, std::string seting);

            void AddSeting(std::string name, setings_seting seting);

            int UpdateSeting(std::string name, std::string input_data);

            int GetSeting(std::string name, setings_seting *seting);

            Setings(/* args */);
            ~Setings();
    }; 


    int Setings::GetSeting(std::string name, setings_seting *seting)
    {
        if(_setings.count(name)) return ERRORS_NO_KEY;

        *seting = _setings[name];

        return ERRORS_OK;
    }

    void Setings::AddSeting(std::string name, int seting)
    {
        setings_seting ss;
        ss.type = _int;
        ss.data._int = seting;
        _setings.insert(std::pair<string, setings_seting>(name,ss));

        auto iterator = _setings.begin();
        
    }

    void Setings::AddSeting(std::string name, bool seting)
    {
        setings_seting ss;
        ss.type = _bool;
        ss.data._bool = seting;
        _setings.insert(std::pair<string, setings_seting>(name,ss));
    }

    void Setings::AddSeting(std::string name, float seting)
    {
        setings_seting ss;
        ss.type = _float;
        ss.data._float = seting;
        _setings.insert(std::pair<string, setings_seting>(name,ss));
    }

    void Setings::AddSeting(std::string name, std::string seting)
    {
        setings_seting ss;
        ss.type = _string;
        ss.data._string = &seting;
        _setings.insert(std::pair<string, setings_seting>(name,ss));
    }

    int Setings::UpdateSeting(std::string name, std::string input_data)
    {
        if(_setings.count(name)) return ERRORS_NO_KEY;
        
        setings_seting ss = _setings[name];
        try
        {
            switch (ss.type)
            {
                case _int:
                    ss.data._int = std::stoi(input_data);
                break;
                case _float:
                    ss.data._float = std::stof(input_data);
                break;
                case _string:
                    ss.data._string = &input_data;
                break;
                case _bool:
                    ss.data._bool = std::stoi(input_data)>0 ? 1 : 0;
                break;
                default: throw; break;
            }
        }
        catch(const std::exception& e){return ERROR_INVALID_CONVERSION;}
        
        return ERRORS_OK;
    }

    Setings::Setings(/* args */){}
    Setings::~Setings(){}
}



