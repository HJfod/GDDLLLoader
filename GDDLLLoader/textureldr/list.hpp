#pragma once

#include <cocos2d.h>

class listManager;

//the list class embodies all elements needed to form a list: a title, the actual list, and navigation buttons.

class list {
protected:
    //for more "understandability" i guess, better than macros i think?
    typedef cocos2d::CCMenuItemSprite* button_t;
    typedef cocos2d::CCLabelBMFont* label_t;
    typedef cocos2d::CCMenu* menu_t;

protected:
    const char* m_titleStr = "";
    label_t m_titleLabel = nullptr;

    std::vector<std::string> m_listStrings = {};
    label_t* m_listLabels = nullptr;
    int m_maxDisplayedLength = 0;
    int m_displayedLength = 0;
    bool m_entered = false;
    float m_x = 0.0f, m_y = 0.0f;

    button_t m_upBtn = nullptr;
    button_t m_downBtn = nullptr;
    void(__stdcall* const m_navFn)(void* pSender);
    int m_listOffset = 0;

    menu_t m_menu = nullptr;

protected:
    inline void getLength() {
        m_displayedLength = static_cast<int>(m_listStrings.size() - m_listOffset) < m_maxDisplayedLength ?
            (m_listStrings.size() - m_listOffset)
            : m_maxDisplayedLength;
    }
    inline void toggle(button_t& button, bool enabled) {
        button->setEnabled(enabled);
        button->setVisible(enabled);
    }
    virtual inline bool isParent(button_t button) { return (button == m_upBtn || button == m_downBtn); }

    virtual inline bool isUp(button_t button) { return (button == m_upBtn); }

    virtual void navigate(bool up);

    virtual void update();

    virtual void enter(cocos2d::CCScene* scene);
    inline void exit() { m_entered = false; }

    virtual bool load(cocos2d::tinyxml2::XMLDocument* file);
    virtual void save(cocos2d::tinyxml2::XMLDocument* file);

public:
    list(const char* title, int length);
    ~list() { delete[] m_listLabels; }

    void setVector(const std::vector<std::string>& vec);
    virtual int ifNotFound(const std::vector<std::string>& other, bool add);
    inline const std::vector<std::string>& getVector() { return m_listStrings; }
    const inline int getCurrentIndex() { return m_listOffset; }
    
    void setPosition(float x, float y) { m_x = x; m_y = y; }
    virtual void setOffset(int offset);

    friend class listManager;
};

//listExt adds swap and move abilities to the list, and also allows for the list to display more than one element at a time.

class listExt : public list {
protected:
    button_t m_swapUpBtn = nullptr;
    button_t m_swapDownBtn = nullptr;
    void (__stdcall* const m_swapFn)(void* pSender);

    button_t m_moveBtn = nullptr;
    void (__stdcall* const m_moveFn)(void* pSender);
    int m_moveIndex = 0;

    listExt* m_target = nullptr;

protected:
    virtual inline bool isParent(button_t button) override {
        return (button == m_upBtn || button == m_downBtn ||
            button == m_swapUpBtn || button == m_swapDownBtn ||
            button == m_moveBtn);
    }
    virtual inline bool isUp(button_t button) override {
        return (button == m_upBtn || button == m_swapUpBtn);
    }

    virtual void navigate(bool up) override;
    void swap(bool up);
    void move();

    void updateLabels();
    void updateSelector();
    virtual inline void update() override {
        updateLabels();
        updateSelector();
    }
    
    virtual void enter(cocos2d::CCScene* scene) override;

    virtual bool load(cocos2d::tinyxml2::XMLDocument* file) override;
    virtual void save(cocos2d::tinyxml2::XMLDocument* file) override;

public:
    listExt(const char* title, int length, listExt* target);
    virtual int ifNotFound(const std::vector<std::string>& other, bool add) override;

    virtual void setOffset(int offset) override;

    friend class listManager;
};

/*listManager keeps track of all lists using a vector, which allows for callbacks to be cleaner and for full encapsulation of functionality.
* although it's probably very slow if you have too many lists or something lol idk
* 
* the alternative was to make my own inherited class from cocos and i dont know how tf that works soooo
*/

class listManager {
private:
    static inline std::vector<list*> m_vec = {};
    static inline cocos2d::tinyxml2::XMLDocument* m_saveFile = nullptr;
    static inline const char* m_filePath = "";
    static inline const char* m_backupPath = "";

private:
    //you shouldn't be able to make instances of this class, as all members are static
    listManager() {};

    static inline void add(list* list) { m_vec.push_back(list); }

    static void __stdcall navigate(void* pSender);
    static void __stdcall swap(void* pSender);
    static void __stdcall move(void* pSender);

public:
    static void enter(cocos2d::CCScene* scene);
    static void exit();
    static inline void setSaveTargets(const char* file, const char* backup) { m_filePath = file; m_backupPath = backup; }
    static bool load();
    static bool save();

    friend class listExt;
    friend class list;
};