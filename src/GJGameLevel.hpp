#include <cocos2d.h>

/*
level=Offsets(
    id=0xF8,
    name=0xFC,
    description=0x114,
    creator_name=0x144,
    unprocessed_data=0x12C,
    difficulty_denominator=0x1E0,
    difficulty_numerator=0x1E4,
    attempts=0x218,
    jumps=0x224,
    normal_percent=0x248,
    practice_percent=0x26C,
    score=0x27C,
    epic=0x280,
    demon=0x29C,
    demon_difficulty=0x2A0,
    stars=0x2AC,
    auto=0x2B0,
    level_type=0x364,
)
*/

struct GJGameLevel : cocos2d::CCNode {
    public:
        cocos2d::CCDictionary* lastBuildSave;

        int levelID_rand; // levelID + seed = levelID_rand
        int levelID_seed;
        int levelID;

        std::string levelName;
        std::string levelDesc;
        std::string levelString;
        std::string userName;
        std::string recordString;

        std::string uploadDate;
        std::string updateDate;

        int userID_rand;
        int userID_seed;
        int userID;

        int accountID_rand;
        int accountID_seed;
        int accountID;
};