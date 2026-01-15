#!/bin/bash

check() {
    # Find files containing the string, prefix with the address
    # We use -l to list filenames, and xargs to format if needed, or just loop
    # grep -lF returns filenames.
    files=$(grep -lF "$2" src/*.cpp src/*.c 2>/dev/null)
    if [ -z "$files" ]; then
        echo "$1: NOT FOUND ($2)"
    else
        for f in $files; do
            echo "$1: $f"
        done
    fi
}

check '0x435050' 'Must define an LBLParse'
check '0x435080' 'SC_ZBuffer::Update'
check '0x4350E0' 'SC_Timer::Update'
check '0x435180' 'quit.smk'
check '0x435284' 'ENTER INTRO GAME TEXT'
check '0x43540C' 'SearchScreen'
check '0x435554' 'SCI_AfterSchoolMenu'
check '0x4356C0' 'SCI_Dialog'
check '0x435840' 'MMPlayer2'
check '0x4358E4' 'T_Hotspot'
check '0x43599C' 'SC_OnScreenMessage::Update'
check '0x435A00' 'SCI_SearchScreen'
check '0x435A34' 'SC_Sound::Update'
check '0x435A60' 'Teacher Demo'
check '0x435EE4' 'SC_Combat1'
check '0x435F40' 'Engine'
check '0x4360AC' 'mCNavNode'
check '0x436224' 'mCNavigator'
check '0x4362D8' 'TargetScene'
check '0x436350' 'CombatSprite'
check '0x4363F0' 'MapScene'
check '0x4364B0' 'Parser::Open'
check '0x4366B8' 'dest string too small'
check '0x43670C' 'message.log'
check '0x4367A8' 'Error Reading CD'
check '0x4369A8' 'barrel06.smk'
check '0x436A98' 'VBuffer::Init'
check '0x436B88' 'declaring ZBuffer'
check '0x436C78' 's_Sprite'
# Special check for Sprite class name string if s_Sprite failed to map to a literal
grep -l '"Sprite"' src/*.cpp | sed 's/^/0x436C78_ALT: /'
check '0x437004' 'MouseControl'
check '0x437084' 'Animation::Open'
check '0x437174' 'Error in flagaray'
