// Singleton,h
#ifndef _SINGLETON

#define _SINGLETON
 template <typename C>
 
class Singleton
 {
 public:
    static C* i ()
    {
       if (!_instance)
	   {
          _instance = new C ();
	   }
       return _instance;
    }
    virtual
    ~Singleton ()
    {
       _instance = 0;
    }
 private:
    static C* _instance;
protected:
    Singleton () { }
 };
 template <typename C> C* Singleton <C>::_instance = 0;

#endif