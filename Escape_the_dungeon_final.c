#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <stdbool.h>

typedef struct
{
    char Name[50];
    int id;
    int primary_damage; //for rolls
    int secondary_damage; //for rolls
    int stat_index; //stat that modifies damage
} weapon;

typedef struct
{
    char Name[50];
    int id;
    int AC;
} armour;

typedef struct
{
    char Name[50];
    int id;
    char Description[5000];
} items;

typedef struct
{
    char Name[50];
    char type[20];
    int effect; //damage or healing
    int mp_cost;
    char description[100];
} spells;

typedef struct
{
    char Name[50];
    int Class; // Warrior, Rogue, Cleric, Mage
    int HP; // Current Health
    int MP; // Current Magic
    int AC; // Armour class
    int spell_slots;
    int stats[6]; // STR, DEX, CON, INT, WIS, CHA
    items inventory[20];
    weapon main_hand;
    armour body_armour;
    spells available_spells[10];
    int accuracy;
    int gold;
    int Health; // Max HP
    int Max_magic; // Max MP
    int level;
    int souls;
} player;

typedef struct
{
    char Name[50];
    int HP;
    int max_hp;
    int AC;
    int accuracy;
    int level;
    int damage_min;
    int damage_max;
    int souls;
} enemy;

//Build presets
void Warrior_preset(player *p);
void Rogue_preset(player *p);
void Cleric_preset(player *p);
void Mage_preset(player *p);

//Enemy seting
void setUndead(enemy *undead, int max);
void setSkeleton(enemy *skeleton, int max);
void setTheWarden(enemy *warden);

//weapons and armour
void create_iron_sword(weapon *Iron_sword, player *p);
void create_dual_daggers(weapon *daggers, player *p);
void create_iron_mace(weapon *mace, player *p);
void create_staff(weapon *staff, player *p);
void create_chainmail(armour *chainmail);
void create_leather_garb(armour *leather);
void create_scholars_robe(armour *robe);

//spells
void create_fireball_spell(spells *fireball);
void create_shadow_blade_spell(spells *shadow_blade);
void create_restoration_spell(spells *restoration);

//items
void healing_potion(items *HP_potion);
void magic_potion(items *MP_potion);
void Wardens_journal_write(items *journal);
void Wardens_journal_read(items *journal);
void add_item(player *p, items new_item);
void use_item(player *p, int index);

//for combat
void single_combat(player *p, enemy *e);
int initiative_single(player *p, enemy *e);
void player_attack(player *p, enemy *e, int *enemy_guard);
void enemy_attack(enemy *e, player *p, int *player_guard);
void cast_spells(player *p, enemy *e);

//narrative
void cls_descriptions();
void prologue(player *p);
void post_combat();
void hub_area();

//hub options
void status(player *p);
void sleep(player *p);
void inventory_menu(player *p);
void level_up(player *p);

//areas
void sewers(player *p, enemy *undead, enemy *skeleton);
void prison(player *p, enemy *undead, enemy *skeleton);
void ritual_site(player *p, enemy *undead, enemy *skeleton);

//save and load
void save_character(player *p);
void load_character(player *p);

int main()
{
    srand(time(NULL));

    player p1;

    printf("Welcome to Escape the dungeon game demo!\n");
    printf("Please play in full-screen for the best experience.\n");

    int menu;
    bool loadstatus = false;

    printf("\nStart Game!\n");
    printf("\n1.Create new character\n2.Load character\n");
    scanf("%d", &menu);
    if(menu <= 0 || menu >= 3)
    {
        printf("\nInvalid Choice!\n");
        return 1;
    }

    else if(menu == 2)
    {
        load_character(&p1);
        loadstatus = true;
    }

    if(loadstatus == true)
    {
        goto start;
    }

    fflush(stdin);

    printf("\nEnter your name: ");
    fgets(p1.Name, 50, stdin);


    if(strlen(p1.Name) > 0 && p1.Name[strlen(p1.Name) - 1] == '\n')
    {
        p1.Name[strlen(p1.Name) - 1] = '\0';
    }

    int class_choice;
    int scanf_result;

    cls_descriptions();

Class_selection:
    printf("\nChoose your class \n1.Warrior\n2.Rogue\n3.Cleric\n4.Mage\n");
    scanf_result = scanf("%d", &class_choice);

    if (scanf_result != 1)
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
        printf("\nError... Please input an integer between 1 and 4...\n");
        goto Class_selection;
    }

    switch(class_choice)
    {
    case 1:
        Warrior_preset(&p1);
        break;
    case 2:
        Rogue_preset(&p1);
        break;
    case 3:
        Cleric_preset(&p1);
        break;
    case 4:
        Mage_preset(&p1);
        break;
    default:
        printf("\nError...Input an integer between 1 and 4...\n");
        goto Class_selection;
    }
    printf("\nCharacter created!\n");

start:
    fflush(stdin);

    printf("\nPress Enter to continue...\n");
    getchar();

    enemy undead[5];
    enemy skeleton[5];

    int level = 5;
    setUndead(&undead, level);
    setSkeleton(&skeleton, level);

    items Hpotion;
    healing_potion(&Hpotion);

    items Mpotion;
    magic_potion(&Mpotion);

    for(int i = 0; i < 3; i++)
    {
        add_item(&p1, Hpotion);
    }

    if(p1.Class == 3 || p1.Class == 4)
    {
        for(int i = 0; i < 2; i++)
        {
            add_item(&p1, Mpotion);
        }
    }

    status(&p1);

    printf("\nPress Enter to continue...\n");
    getchar();

    prologue(&p1);

    single_combat(&p1, &undead[0]);

    if(p1.HP <= 0)
    {
        return 0;
    }

    post_combat();
    hub_area();

    items journal;
    Wardens_journal_write(&journal);
    Wardens_journal_read(&journal);
    add_item(&p1, journal);

    enemy warden;

hub:
    while(true)
    {
        int action;

        printf("\nWhat do you want to do next?\n");
        printf("1.Sleep(recover HP and MP)\n2.See Status\n3.Inventory\n4.Level Up\n5.Head Outside\n6.Save character\n");
        scanf("%d", &action);

        switch(action)
        {
        case 1:
        {
            sleep(&p1);
            break;
        }
        case 2:
        {
            status(&p1);
            break;
        }
        case 3:
        {
            inventory_menu(&p1);
            break;
        }
        case 4:
        {
            level_up(&p1);
            break;
        }
        case 5:
        {
            if(p1.level < 5)
            {
                printf("\nYou don't feel prepared to confront whatever is upstairs, given by the sounds you heard from above.\n");
                getchar();
            }
            else if(p1.level >= 5)
            {
                int bossfight;
                printf("\nYou feel like you have gained the minimum strength necessary to confront whatever abomination lives upstairs.\n");
                getchar();
                printf("\nHead upstairs?(Warning! You might not be able to come back here)\n\n1.Yes\n2.Not yet");
                scanf("%d", &bossfight);

                switch(bossfight)
                {
                case 1:
                {
                    setTheWarden(&warden);
                    printf("\nSteeling yourself, You decide to brace the unknown and head upstairs to confront whatever is there\n");
                    getchar();
                    printf("\nAs you climb up the spiral stairs, you feel a weird creeping sensation in your back.\n");
                    getchar();
                    printf("\nYou shake off the feeling and focus on the task ahead\n");
                    getchar();
                    printf("\nAs you make your way to the ground floor, you are hit with the stench of decaying corpses.\n");
                    getchar();
                    printf("\nYou quickly find the source of the reek as you look at the courtyard near the gates of the castle.\n");
                    getchar();
                    printf("\nThere lays a pile of mutilated bodies, and above them is a man in a knights armour hunching over, gorging on the corpses.\n");
                    getchar();
                    printf("\nThe monstrosity takes notice of you, its red eyes glowing.\nThe abomination stood a head taller than you, it had elongated fingers, and its flesh had a reddish hue");
                    getchar();
                    printf("\nThe monstrosity picks up a spear that still had one of the guards impaled on it.\nIt roars at you and charges in your direction\n");

                    single_combat(&p1, &warden);

                    if(p1.HP <= 0)
                    {
                        return 0;
                    }
                    else
                    {
                        printf("\nAfter a gruelling battle, you finally manage to kill the abomination, who you presume to be the warden of the castle.\n");
                        getchar();
                        printf("\nYou heave a sigh of exhaustion and relief amidst a field filled with the mutilated remains of those who failed to achieve what you did.\n");
                        getchar();
                        printf("\nYou staggered up to the massive doors leading outside this castle.This hellhole.\n");
                        getchar();
                        printf("\nYou use all your strength to push open the doors, feeling a breath of fresh air after god knows how long.\n");
                        getchar();
                        printf("\nYou take one last long look behind you, into the field, the stairs leading to the dungeons where untold horrors remain in the depths of the dungeon.\n");
                        getchar();
                        printf("\nAnd then you take the step forward, out of the dungeon, out of the castle, FREE\n");
                        getchar();
                        printf("\nCongratulations! You have escaped the dungeon!\n");
                        printf("\nThank you for playing this game!\n");
                        return 0;
                    }
                }
                case 2:
                {
                    printf("\nYou don't feel confident enough to confront that thing. you decide to take your time and prepare.\n");
                    break;
                }
                default:
                {
                    printf("\nInvalid Choice!\n");
                    goto hub;
                    break;
                }
                }
            }
            printf("\nLooking at the map of the castle, you note a couple of points of interest.\n");
            getchar();
            printf("\nYou could check out the Sewers, see if you can find a way out through there.\n");
            getchar();
            printf("\nOr you could check out the prison cells, to see if you could find any supplies.\n");
            getchar();
            printf("\nOr maybe you could go deeper into the dungeon, in the ritual site, to see if you could uncover the mysteries of what happened here.");
            getchar();

            int ar_choice;
            printf("\nSo where will you go?\n\n1.Sewers\n2.Prison\n3.Ritual Site\n");
            scanf("%d", &ar_choice);

            switch(ar_choice)
            {
            case 1:
            {
                sewers(&p1, &undead[rand() % 3 + 0], &skeleton[rand() % 3 + 0]);
                if(p1.HP  <= 0)
                {
                    return 0;
                }
                break;
            }
            case 2:
            {
                prison(&p1, &undead[rand() % 4 + 0], &skeleton[rand() % 3 + 0]);
                if(p1.HP  <= 0)
                {
                    return 0;
                }
                break;
            }
            case 3:
            {
                ritual_site(&p1, &undead[rand() % 4 + 0], &skeleton[rand() % 4 + 0]);
                if(p1.HP  <= 0)
                {
                    return 0;
                }
                break;
            }
            default:
            {
                printf("\nInvalid Choice\n");
                break;
            }

            }
            break;
        }
            case 6:
                {
                    save_character(&p1);
                    break;
                }
        default:
        {
            printf("\nInvalid Choice!\n");
            break;
        }
        }
    }

    return 0;
}

void cls_descriptions()
{
    printf("\nThere are 4 possible classes in this demo..\n");
    printf("\nThe Warrior: Be a soldier from the frontlines of combat. Warriors are melee-based class that thrives in head to head battles.");
    printf("\nThe Rogue: Be an assassin from the underworld. Rogues are sneaky, melee-based class that uses tricks and underhanded means to win battles");
    printf("\nThe Cleric: Be a priest from the church of Hellion the sun god. Clerics are primarily healers, though they can hold their own in a fight.");
    printf("\nThe Mage: Be a wizard from the Tower of Ramazith. Mages mostly rely on spells to attack, so they are a ranged class, though they can fight in melee when needed\n");
}

void prologue(player *p)
{

    printf("\nYou wake up feeling groggy...You don't remember where you came from\n\nNor do you remember how you ended up here..\n");
    getchar();
    printf("\nYou take a look at your current situation.\n");
    getchar();
    printf("\nYou are laying on what seems to be a bed made of stone, In the middle of a room completely devoid of any light\n");
    getchar();
    printf("\nYou try to stand up and take a look around you to asses your current situation.\n");
    getchar();

    switch(p->Class)
    {
    case 1:
    {
        printf("\nYou feel the heavy Chainmail you are wearing, and the sword at your waist.\n");
        getchar();
        printf("\nSome memories rush back into your psyche, you remember fighting, the loud noises and the fire, and a battlefield filled with vultures gnawing on corpses\n");
        getchar();
        printf("\nYour heavy armour and the sword at you waist gives you some feeling of solace in this unknown place.\n");
        getchar();
        printf("\nWithout any light, you stumble around in the room until you chance upon a door which you think is the way outside the room\n");
        getchar();
        break;
    }
    case 2:
    {
        printf("\nYou feel the leather armour you are wearing and the daggers in your belt.\n");
        getchar();
        printf("\nSome memories rush back into your psyche, you remember walking along a dark alley in the city, then you remember reaching behind the walls of an exotic mansion.\n");
        getchar();
        printf("\nYou remember your dagger slitting the throat of a man wearing extravagant clothes, in the middle of a posh bedroom. His throat making gurgling sounds as he falls limp to the ground\n");
        getchar();
        printf("\nYou draw the dagger from your belt...It's familiar handle provides some solace in this unknown place.\n");
        getchar();
        printf("\nWithout any light, you stumble around in the room until you chance upon a door which you think is the way outside the room\n");
        getchar();
        break;
    }
    case 3:
    {
        printf("\nYou feel the heavy chainmail that you are wearing and the weight of your iron mace at your back\n");
        getchar();
        printf("\nSome memories rush back into your psyche, you remember the glistening pillars inside a church, and the massive statue of a man whose face is eclipsed by the sun.\n");
        getchar();
        printf("\nYou remember praying at the statue, in the name of Hellion\n");
        getchar();
        printf("\nYou offer a silent prayer to Hellion for protection in this foul and wretched place you find yourself in, and to bestow his light on you to see the way.\n");
        getchar();
        printf("\nThe prayer produces a small ball of light in front of you which clears the darkness in the room and reveals itself to you.");
        getchar();
        printf("\nYou notice the floor of the room is covered in dried blood, otherwise, the room is empty except for a stone slab where you woke up.\n");
        getchar();
        printf("\nA ritualistic hexagram in the shape of two triangles intertwined with each other with the symbol of an eye in the middle of the diagram catches your attention.\n");
        getchar();
        printf("\n\"Belial\" You curse to yourself. You draw your mace and strike at the symbol, desecrating it, before you make your way over to the door leading out of the room.\n");
        getchar();
        break;
    }
    case 4:
    {
        printf("You feel your robe which still seems to retain its colour, and the weight of your staff at your back");
        getchar();
        printf("\nSome memories rush back into your psyche, you remember a massive tower standing tall, piercing through the clouds.\n");
        getchar();
        printf("\nYou remember yourself pouring over the books in a library, absorbing knowledge on anything you can get your hands on.\n");
        getchar();
        printf("\nYou mutter an arcane incantation which summons a ball of light in front of you, lighting up the room");
        getchar();
        printf("\nA ritualistic hexagram in the shape of two triangles intertwined with each other with the symbol of an eye in the middle of the diagram catches your attention.\n");
        getchar();
        printf("\n\"Mark of Belial...Unusual...\"You mutter to yourself. you look around the room and notice that the floor is covered in dried blood.\n");
        getchar();
        printf("\nYou carefully step to the door leading out of the room, making sure not to step on lumps of congealed blood.");
        getchar();
        break;
    }
    }

    printf("\nYou find yourself in what appears to be hallway of a castle dungeon.\n");
    getchar();
    printf("\nYou look at the walls of the hallway which is made with stone bricks. The walls have splatters of blood all over the hallway.\n");
    getchar();
    printf("\nYou notice a couple of corpses of what you assume to be guards laying on the floor or against the wall.");
    getchar();

    if(p->Class == 1)
    {
        printf("\nAs you look at the corpses of the guards and the hallway, you get a splitting headache. You feel like you have been to this place before.\n");
        getchar();
    }

    printf("\nAs you were closing in on a corpse to inspect one of the cadavers, the corpse suddenly jerks violently and stands up.\n");
}

void post_combat()
{
    printf("\nAs you finish dealing with the undead, you look around and see that the other corpses in the hallway also seemed to be waking up from the commotion.\n");
    getchar();
    printf("\nKnowing that you can't possibly fight all of them at once, you start running away from the undead.\n");
    getchar();
    printf("\nOne by one, the dead wake up and starts chasing you...You hear the restless footsteps of the undead behind you, as well as a rumbling sound from above the ceiling.\n");
    getchar();
    printf("\nYou keep running through the twisting hallway of the castle...as you were about to lose hope of respite from the undead, you spot something unusual.\n");
    getchar();
    printf("\nYou see what you presume to be the office of the warden of this dungeon, just beside the stairs leading up..\n");
    getchar();
    printf("\nThere is something like a warding rune that is placed on the door to the warden's office, which seemed to repel the undead as none were willing to go close to it.\n");
    getchar();
    printf("\nSince whatever was upstairs did not seem like good news, you decided not to take any chances and instead rushed inside the warden's office, slamming the door shut immediately.\n");

}
void hub_area()
{
    printf("\nAs you looked around the warden's office, you note that it is more neatly oriented than you would expect given the situation outside.\n");
    getchar();
    printf("\nThe room is somewhat modest, with a large wooden desk on one side, behind the desk there are bookshelves. The floor is covered in a blue carpet that seemed relatively well kept.\n");
    getchar();
    printf("There is a cabinet opposite to the desk as well as a safe directly behind the desk.\n");
    getchar();
    printf("\nYou carefully check the room for any traps or unexpected survivors.\n");
    getchar();
    printf("\nThough you don't find any adversaries or traps, you also don't find any survivors that could tell you about the situation here.\n");
    getchar();
    printf("\nYou go through the desk and cabinet to find anything that could help you.\n");
    getchar();
    printf("\nMost of the papers you find are just logistics of the castle or files on prisoners.\n");
    getchar();
    printf("\nAfter combing through the entire room, you find a map of the castle as well as the warden's journal.\n");
    getchar();
    printf("\nSomewhat satisfied with your find, you sit on the desk and unfurl the map to ponder on your next move.\n");
    getchar();
}

void Warrior_preset(player *p)
{
    p->Class = 1;
    p->spell_slots = 1;
    int stats[] = {16, 13, 15, 12, 10, 8};
    memcpy(p->stats, stats, sizeof(stats));
    p->Health = 100 + ((p->stats[2]) - 5);
    p->Max_magic = 50 + ((p->stats[4]) - 5);
    p->HP = p->Health;
    p->MP = p->Max_magic;
    p->accuracy = 6;
    p->level = 1;
    p->souls = 0;

    for(int i = 0; i < 20; i++)
    {
        p->inventory[i].id = 0;
    }

    memset(p->available_spells, 0, sizeof(p->available_spells));

    create_iron_sword(&p->main_hand, p);

    create_chainmail(&p->body_armour);
    p->AC = p->body_armour.AC;
}

void Rogue_preset(player *p)
{
    p->Class = 2;
    p->spell_slots = 2;
    int stats[] = {13, 16, 15, 12, 10, 8};
    memcpy(p->stats, stats, sizeof(stats));
    p->Health = 90 + ((p->stats[2]) - 5);
    p->Max_magic = 60 + ((p->stats[4]) - 5);
    p->HP = p->Health;
    p->MP = p->Max_magic;
    p->accuracy = 8;
    p->level = 1;
    p->souls = 0;

    for(int i = 0; i < 20; i++)
    {
        p->inventory[i].id = 0;
    }

    memset(p->available_spells, 0, sizeof(p->available_spells));

    create_dual_daggers(&p->main_hand, p);

    create_leather_garb(&p->body_armour);
    p->AC = p->body_armour.AC;

    spells ShadowBlade;
    create_shadow_blade_spell(&ShadowBlade);
    p->available_spells[0] = ShadowBlade;
}

void Cleric_preset(player *p)
{
    p->Class = 3;
    p->spell_slots = 4;
    int stats[] = {12, 11, 15, 12, 14, 10};
    memcpy(p->stats, stats, sizeof(stats));
    p->Health = 80 + ((p->stats[2]) - 5);
    p->Max_magic = 70 + ((p->stats[4]) - 5);
    p->HP = p->Health;
    p->MP = p->Max_magic;
    p->accuracy = 5;
    p->level = 1;
    p->souls = 0;

    for(int i = 0; i < 20; i++)
    {
        p->inventory[i].id = 0;
    }

    memset(p->available_spells, 0, sizeof(p->available_spells));

    create_iron_mace(&p->main_hand, p);

    create_chainmail(&p->body_armour);
    p->AC = p->body_armour.AC;

    spells Fireball, Restoration;
    create_fireball_spell(&Fireball);
    create_restoration_spell(&Restoration);
    p->available_spells[0] = Restoration;
    p->available_spells[1] = Fireball;
}

void Mage_preset(player *p)
{
    p->Class = 4;
    p->spell_slots = 5;
    int stats[] = {12, 10, 15, 16, 13, 8};
    memcpy(p->stats, stats, sizeof(stats));
    p->Health = 80 + ((p->stats[2]) - 5);
    p->Max_magic = 70 + ((p->stats[4]) - 5);
    p->accuracy = 7;
    p->HP = p->Health;
    p->MP = p->Max_magic;
    p->level = 1;
    p->souls = 0;

    for(int i = 0; i < 20; i++)
    {
        p->inventory[i].id = 0;
    }

    memset(p->available_spells, 0, sizeof(p->available_spells));

    create_staff(&p->main_hand, p);

    create_scholars_robe(&p->body_armour);
    p->AC = p->body_armour.AC;

    spells Fireball, Restoration, ShadowBlade;
    create_fireball_spell(&Fireball);
    create_restoration_spell(&Restoration);
    create_shadow_blade_spell(&ShadowBlade);
    p->available_spells[0] = Restoration;
    p->available_spells[1] = Fireball;
    p->available_spells[2] = ShadowBlade;
}

void level_up(player *p)
{
    if (p->level >= 10)
    {
        printf("\nYou've reached the maximum level (10)!\n");
        return;
    }

    int souls_needed = 100 * p->level;
    if (p->souls < souls_needed)
    {
        printf("\nYou need %d souls to reach level %d. You have %d.\n",
               souls_needed, p->level + 1, p->souls);
        return;
    }


    p->souls -= souls_needed;
    p->level++;

    printf("\nCongratulations! You leveled up to level %d!\n", p->level);

    switch(p->Class)
    {
    case 1:
    {
        p->stats[0] += 1; // str
        p->stats[2] += 1; // con
        break;
    }
    case 2:
    {
        p->stats[1] += 1; // dex
        p->stats[2] += 1; // con
        break;
    }
    case 3:
    {
        p->stats[4] += 1; // wis
        p->stats[2] += 1; // con
        break;
    }
    case 4:
    {
        p->stats[3] += 1; // int
        p->stats[1] += 1; // dex
        break;
    }
    }

    int stat;
    printf("\nChoose an attribute to improve.(Bonus choice)\n");
    printf("\n1.Strength\n2.Dexterity\n3.Constitution\n4.Intelligence\n5.Wisdom\n6.Charisma\n\nChoice: ");
    scanf("%d", &stat);

    p->stats[stat - 1] += 1;

    p->Health += 10 + (p->stats[2] - 10)/2;
    p->Max_magic += 5 + (p->stats[4] - 10)/2;

    p->HP = p->Health;
    p->MP = p->Max_magic;

    printf("Your maximum HP is now %d and MP is %d!\n", p->Health, p->Max_magic);
}

void sleep(player *p)
{
    printf("\nYou decide to call it a day and go to sleep.\n");
    printf("\nYou lay on the carpet to get some warmth. You think about your next objectives as you doze off...");

    p->HP = p->Health;
    p->MP = p->Max_magic;

    printf("\nYou wake up the next day feeling refreshed and ready to tackle whatever lays ahead.\n");
}

void status(player *p)
{
    printf("\n\n--- Status ---\n\n");
    printf("\nName: %s\n", p->Name);
    printf("Souls: %d\n", p->souls);
    printf("Level: %d\n", p->level);
    printf("Class: ");
    switch(p->Class)
    {
    case 1:
        printf("Warrior\n");
        break;
    case 2:
        printf("Rogue\n");
        break;
    case 3:
        printf("Cleric\n");
        break;
    case 4:
        printf("Mage\n");
        break;
    }
    printf("HP: %d/%d\n", p->HP, p->Health);
    printf("MP: %d/%d\n", p->MP, p->Max_magic);
    printf("AC: %d\n", p->AC);
    printf("Accuracy: %d\n", p->accuracy);
    printf("\nEquipped Weapon: %s (Damage: %d - %d)\n", p->main_hand.Name, (2 + (p->stats[p->main_hand.stat_index] - 10) / 2)
           , ((p->main_hand.primary_damage + p->main_hand.secondary_damage) + (p->stats[p->main_hand.stat_index] - 10) / 2));
    printf("Equipped Armor: %s (AC: %d)\n", p->body_armour.Name, p->body_armour.AC);
    printf("\nAttributes:\n");
    printf("STR: %d\nDEX: %d\nCON: %d\nINT: %d\nWIS: %d\nCHA: %d\n",
           p->stats[0], p->stats[1], p->stats[2], p->stats[3], p->stats[4], p->stats[5]);
}

void setTheWarden(enemy *warden)
{
    strcpy(warden->Name, "Cedric Varn The Warden");
    warden->HP = 200;
    warden->AC = 16;
    warden->max_hp = warden->HP;
    warden->accuracy = 10;
    warden->level = 7;
    warden->damage_min = 8;
    warden->damage_max = 14;
    warden->souls = 800;
}

void setUndead(enemy *undead, int max)
{

    for(int i = 0; i < max; i++)
    {
        undead[i].HP = 60 + (15 * i);
        undead[i].AC = 15;
        undead[i].max_hp = undead[i].HP;
        strcpy(undead[i].Name, "Undead Guard");
        undead[i].accuracy = 8;
        undead[i].level = 1 + i;
        undead[i].damage_min = 4;
        undead[i].damage_max = 10;
        undead[i].souls = 60 + (20 * i);
    }
    undead[4].damage_max = 12;
}

void setSkeleton(enemy *skeleton, int max)
{
    for(int i = 0; i < max; i++)
    {
        skeleton[i].HP = 40 + (10 * i);
        skeleton[i].AC = 13;
        skeleton[i].max_hp = skeleton[i].HP;
        strcpy(skeleton[i].Name, "Skeleton soldier");
        skeleton[i].accuracy = 10;
        skeleton[i].level = 1 + i;
        skeleton[i].damage_min = 6;
        skeleton[i].damage_max = 8;
        skeleton[i].souls = 50 + (15 *i);
    }
    skeleton[4].damage_max = 10;
}

void create_iron_sword(weapon *Iron_sword, player *p)
{
    strcpy(Iron_sword->Name, "Iron Sword");
    Iron_sword->id = 1;
    Iron_sword->primary_damage = 10;
    Iron_sword->secondary_damage = 8;
    Iron_sword->stat_index = 0;
}

void create_dual_daggers(weapon *daggers, player *p)
{
    strcpy(daggers->Name, "Dual Daggers");
    daggers->id = 2;
    daggers->primary_damage = 6;
    daggers->secondary_damage = 6;
    daggers->stat_index = 1;
}

void create_iron_mace(weapon *mace, player *p)
{
    strcpy(mace->Name, "Iron Mace");
    mace->id = 3;
    mace->primary_damage = 10;
    mace->secondary_damage = 4;
    mace->stat_index = 0;
}

void create_staff(weapon *staff, player *p)
{
    strcpy(staff->Name, "Magician's Staff");
    staff->id = 4;
    staff->primary_damage = 8;
    staff->secondary_damage = 6;
    staff->stat_index = 1;
}

void create_chainmail(armour *chainmail)
{
    strcpy(chainmail->Name, "Chainmail");
    chainmail->id = 1;
    chainmail->AC = 16;
}

void create_leather_garb(armour *leather)
{
    strcpy(leather->Name, "Leather Garb");
    leather->id = 2;
    leather->AC = 12;
}

void create_scholars_robe(armour *robe)
{
    strcpy(robe->Name, "Scholar's Robe");
    robe->id = 3;
    robe->AC = 11;
}

void create_fireball_spell(spells *fireball)
{
    strcpy(fireball->Name, "Fireball");
    strcpy(fireball->type, "Attack");
    fireball->effect = 15;
    strcpy(fireball->description, "Launches a fiery projectile dealing 15 damage.");
    fireball->mp_cost = 10;
}

void create_shadow_blade_spell(spells *shadow_blade)
{
    strcpy(shadow_blade->Name, "Shadow Blade");
    strcpy(shadow_blade->type, "Attack");
    shadow_blade->effect = 17;
    strcpy(shadow_blade->description, "Conjures a blade from shadows for 17 damage.");
    shadow_blade->mp_cost = 15;
}

void create_restoration_spell(spells *restoration)
{
    strcpy(restoration->Name, "Restoration");
    strcpy(restoration->type, "Heal");
    restoration->effect = 20;
    strcpy(restoration->description, "Heals 20 HP.");
    restoration->mp_cost = 10;
}

void healing_potion(items *HP_potion)
{
    strcpy(HP_potion->Name, "Healing Potion");
    HP_potion->id = 1;
    strcpy(HP_potion->Description, "A small vial of arcane medicine. Restores 20 HP.");
}

void magic_potion(items *MP_potion)
{
    strcpy(MP_potion->Name, "Magic potion");
    MP_potion->id = 2;
    strcpy(MP_potion->Description, "A small vial of arcane medicine. Restores 10 MP.");
}

void Wardens_journal_write(items *journal)
{
    const char *log_entries =
        "// WARDEN'S PERSONAL LOG //\n"
        "// Property of Cedric Varn, Keeper of the Blackstone Citadel //\n\n"
        "/// 12th of Frostmarch ///\n"
        "His Majesty has granted the alchemists unrestricted access to the lower dungeons.\n"
        "I protested - this is a military prison, not a plague hospital - but the Royal Decree\n"
        "leaves no room for refusal. They say the Crimson Flux has reached the capital.\n"
        "The court physician's last report claimed 500 souls lost in the merchant quarter alone.\n\n"
        "/// 17th of Frostmarch ///\n"
        "The first prisoners were taken to the alchemical chambers today. Master Albrecht\n"
        "assures me this is necessary for the greater good. Their screams echo through the\n"
        "ancient stones like damned spirits. I've ordered the guards to double the watch -\n"
        "whatever they're brewing in those vats makes the rats... twitch.\n\n"
        "/// 24th of Frostmarch ///\n"
        "The alchemists demanded fresh subjects. Not criminals - healthy prisoners. I provided\n"
        "three poachers from the king's forest. When I confronted Albrecht about the black\n"
        "smoke rising from their ritual brazier, he showed me vials of glowing serum.\n"
        "\"The price of salvation\" he called it. The way his shadow moved... wrong.\n\n"
        "/// 1st of Seedmoon ///\n"
        "The lower cells have gone silent. Guards report hearing wet crunching noises after\n"
        "midnight. Albrecht has barred me from the experiment chambers. This morning I found\n"
        "a scrap of parchment with eldritch symbols in the west stairwell - the same markings\n"
        "I saw them paint with blood on the floors.\n\n"
        "/// 5th of Seedmoon ///\n"
        "THEY'RE NOT DEAD. The poacher we gave them last week - Jorik, his name was Jorik -\n"
        "came shambling to the inner gate at dawn. His eyes glowed like hot coals and his\n"
        "flesh... Saints preserve us, his flesh MOVED. Albrecht laughed when I drew steel.\n"
        "\"The cure works better than expected!\" The fool doesn't realize we're not\n"
        "controlling the cure. It's controlling us.\n\n"
        "/// (Last entry, bloodstained and illegible except for final lines) ///\n"
        "The runes were containment. We broke the seals. The serum isn't medicine -\n"
        "it's a key. They're in the walls now. Can't reach the king. Albrecht's\n"
        "body... keeps trying to speak. The prisoners are awake but not alive.\n"
        "Forgive me. Forgive us all. It comes. It- \n\n"
        "The writing cuts off abruptly.";

    FILE *fptr_WJ;
    fptr_WJ = fopen("Wardens Journal.txt", "w");

    fprintf(fptr_WJ,"%s", log_entries);

    fclose(fptr_WJ);
}

void Wardens_journal_read(items *journal)
{
    strcpy(journal->Name, "Warden's Journal");
    journal->id = 3;
    journal->Description[0] = '\0';

    FILE *pWJ = fopen("Wardens Journal.txt", "r");

    char temp[5000];
    while(fgets(temp, sizeof(temp), pWJ))
    {
        strcat(journal->Description, temp);
    }
    fclose(pWJ);
}

void clear_input_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void inventory_menu(player *p)
{
    int choice;
    while(1)
    {
        printf("\n=== Inventory ===\n");
        int item_count = 0;

        for(int i = 0; i < 20; i++)
        {
            if(p->inventory[i].id != 0)
            {
                item_count++;
                printf("%d. %s\n", item_count, p->inventory[i].Name);
            }
        }

        if(item_count == 0)
        {
            printf("Inventory is empty!\n");
            return;
        }

        printf("\n0. Exit\n");
        printf("Choose an item (1-%d): ", item_count);

        if(scanf("%d", &choice) != 1)
        {
            clear_input_buffer();
            printf("Invalid input! Please enter a number.\n");
            continue;
        }

        clear_input_buffer();

        if(choice == 0) return;

        if(choice < 1 || choice > item_count)
        {
            printf("Invalid selection!\n");
            continue;
        }

        int current = 0;
        for(int i = 0; i < 20; i++)
        {
            if(p->inventory[i].id != 0)
            {
                current++;
                if(current == choice)
                {
                    use_item(p, i);
                    break;
                }
            }
        }
    }
}

void add_item(player *p, items new_item)
{
    for (int i = 0; i < 20; i++)
    {
        if (p->inventory[i].id == 0)
        {
            p->inventory[i] = new_item;
            printf("Added %s to inventory.\n", new_item.Name);
            return;
        }
    }
    printf("Inventory is full. Cannot add %s.\n", new_item.Name);
}

void use_item(player *p, int index)
{
    items *item = &p->inventory[index];

    printf("\nUsing %s...\n", item->Name);

    switch(item->id)
    {
    case 1: // Healing Potion
    {
        p->HP += 20;
        if(p->HP > p->Health)
        {
            p->HP = p->Health;
        }
        printf("\nHealed 20 HP! Current HP: %d/%d\n", p->HP, p->Health);
        item->id = 0;
        break;
    }
    case 2: //Magic Potion
    {
        p->MP += 10;
        if(p->MP > p->Max_magic)
        {
            p->MP = p->Max_magic;
        }
        printf("\nRegained 10 MP! Current MP: %d/%d\n", p->MP, p->Max_magic);
    }

    case 3: //Journal
    {
        printf("%s\nDescription: %s\n", item->Name, item->Description);
        printf("\nPress enter to continue...\n");
        getchar();
        break;
    }
    default:
        printf("This item can't be used directly.\n");
    }
}

void single_combat(player *p, enemy *e)
{
    int player_guard = 0, enemy_guard = 0;

    int has_spells = 0;
    for (int i = 0; i < 10; i++)
    {
        if (strlen(p->available_spells[i].Name) > 0)
        {
            has_spells = 1;
            break;
        }
    }

    int turn = initiative(&p, &e);

    printf("\n%s goes first!\n\n", turn ? p->Name : e->Name);

    while (p->HP > 0 && e->HP > 0)
    {
        if (turn)
        {
            int choice;
            do
            {
                printf("\n1.Attack\n2.Guard\n3.Spells\n4.Items\n5.Escape\nChoice: ");
                scanf("%d", &choice);

                switch(choice)
                {
                case 1:
                {
                    player_guard = 0;
                    player_attack(p, e, &enemy_guard);
                    turn = 0;
                    break;
                }
                case 2:
                {
                    player_guard = 1;
                    printf("\n%s is guarding!", p->Name);
                    turn = 0;
                    break;
                }
                case 3:
                {
                    if (has_spells)
                    {
                        cast_spell(p, e);
                        turn = 0;
                    }
                    else
                    {
                        printf("\nNo Spells!\n");
                    }
                    break;
                }
                case 4:
                {
                    inventory_menu(p);
                    turn = 0;
                    break;
                }
                case 5:
                {
                    if(p->Class == 2)
                    {
                        printf("\nEscape successful!\n");
                        return;
                    }
                    int escape_chance = rand() % 20 + 1;
                    if(escape_chance >= 10)
                    {
                        printf("\nEscape successful!\n");
                        return;
                    }
                    else
                    {
                        printf("\nEscape failed! The enemy is too quick!\n");
                    }
                    break;
                }
                default:
                    printf("\nInvalid choice!\n");
                }
            }
            while (choice < 1 || choice > 3);
        }
        else
        {
            if (rand() % 2)
            {
                enemy_guard = 0;
                enemy_attack(e, p, &player_guard);
            }
            else
            {
                enemy_guard = 1;
                printf("\n%s is guarding!", e->Name);
            }
            turn = 1;
        }
    }

    if(p->HP > 0)
    {
        printf("\nCongratulations! You won the battle!\n");
        printf("\nYou gain %d souls\n", e->souls);
        p->souls += e->souls;
    }
    else
    {
        printf("\nYou Died!\n\nGame Over\n");
    }
}



int initiative(player *p, enemy *e)
{
    int p_roll, e_roll;
    do
    {
        p_roll = rand() % 20 + 1;
        if (p->Class == 1)
        {
            p_roll += 5;
        }
        else if(p->Class == 2)
        {
            p_roll += 7;
        }
        else if(p->Class == 3)
        {
            p_roll += 2;
        }
        else if(p->Class == 4)
        {
            p_roll += 3;
        }
        e_roll = rand() % 20 + 1;
    }
    while (p_roll == e_roll);

    return (p_roll > e_roll) ? 1 : 0;
}

void player_attack(player *p, enemy *e, int *enemy_guard)
{
    int attack_roll = (rand() % 20 + 1) + p->accuracy;
    printf("\n%s attacks with %d vs AC %d: ", p->Name, attack_roll, e->AC);

    if(attack_roll >= e->AC)
    {
        int primary_damage = rand() % p->main_hand.primary_damage + 1;
        int secondary_damage = rand() % p->main_hand.secondary_damage + 1;
        int stat_mod = (p->stats[p->main_hand.stat_index] - 10) / 2;
        int total_damage = primary_damage + secondary_damage + stat_mod;

        if(*enemy_guard)
        {
            total_damage = (total_damage + 1) / 2;
            printf(" (Guarded)");
        }

        e->HP -= total_damage;
        printf("\nHit! %d damage. %s HP: %d\n",total_damage, e->Name, e->HP);

    }
    else
    {
        printf("\nMiss!\n");
    }
}

void enemy_attack(enemy *e, player *p, int *player_guard)
{
    int attack_roll = (rand() % 20 + 1) + e->accuracy;
    printf("\n%s attacks with %d vs AC %d:", e->Name, attack_roll, p->AC);

    if(attack_roll >= p->AC)
    {
        int damage1 = rand() % (e->damage_max) + 1;
        int damage2 = rand() % (e->damage_min) + 1;
        int total_damage = damage1 + damage2 + e->level;

        if(*player_guard)
        {
            total_damage = (total_damage + 1) / 2;
            printf(" (Guarded)");
        }
        else
        {
            printf(" (Unguarded)");
        }

        p->HP -= total_damage;
        printf("\nHit! %d damage. %s HP: %d\n",total_damage, p->Name, p->HP);
    }
    else
    {
        printf("\nMiss!\n");
    }
}


void cast_spell(player *p, enemy *e)
{
    printf("\nAvailable Spells:\n");
    int spell_count = 0;
    spells available[10];

    for (int i = 0; i < 10; i++)
    {
        if (strlen(p->available_spells[i].Name) > 0)
        {
            printf("%d. %s (%d MP) - %s\n",spell_count+1,p->available_spells[i].Name,p->available_spells[i].mp_cost,p->available_spells[i].description);
            available[spell_count] = p->available_spells[i];
            spell_count++;
        }
    }

    if (spell_count == 0)
    {
        printf("No spells available!\n");
        return;
    }

    int choice;
    printf("Choose a spell (1-%d) or 0 to cancel: ", spell_count);
    scanf("%d", &choice);

    if (choice < 1 || choice > spell_count)
    {
        printf("Invalid choice!\n");
        return;
    }

    spells selected = available[choice-1];
    if (p->MP < selected.mp_cost)
    {
        printf("Not enough MP!\n");
        return;
    }

    p->MP -= selected.mp_cost;
    printf("\n%s casts %s!\n", p->Name, selected.Name);

    if (strcmp(selected.type, "Attack") == 0)
    {
        e->HP -= selected.effect;
        printf("%s takes %d damage!\n", e->Name, selected.effect);
    }
    else if (strcmp(selected.type, "Heal") == 0)
    {
        p->HP += selected.effect;
        if (p->HP > p->Health)
        {
            p->HP = p->Health;
        }
        printf("%s heals %d HP!\n", p->Name, selected.effect);
    }
}
void sewers(player *p, enemy *undead, enemy *skeleton)
{
    undead->HP = undead->max_hp;
    skeleton->HP = skeleton->max_hp;

    printf("\nYou decide to head to the sewers, to see if you can find a way out");
    getchar();
    printf("\nYou sneak by the undead when heading out of the warden's office\n");
    getchar();
    printf("\nYou are hit with the acrid stench of the sewers as soon as you step in through the door to the sewers");
    getchar();
    printf("\nYou see that the sewer is not empty and is instead crawling with strange rats along with some undead\n");
    getchar();
    printf("\nOne of the undeads spot you!\n");

    single_combat(p, undead);
    if(p->HP > 0)
    {
        printf("\nBefore you have time to gather your mettles from fighting the undead, you spot a skeleon charging at you, drawn in by the sounds of battle\n");
        getchar();
        single_combat(p, skeleton);
    }
    if (p->HP > 0)
    {
        printf("\nYou loot their bodies to see if you can find something.\n");
        getchar();
        items Hpotion;
        healing_potion(&Hpotion);
        add_item(p, Hpotion);
        printf("\nYou follow the path beside the sewer drainage for a while.\n");
        getchar();
        printf("\nEventually you come up on a huge hole in the sewer that leads into the dark abyss below\n");
        getchar();
        printf("\nThere is no way below aside from jumping in and you don't see how that would help you in escaping so you turn back and head back to the warden's office\n");
        getchar();
    }
}

void prison(player *p, enemy *undead, enemy *skeleton)
{
    undead->HP = undead->max_hp;
    skeleton->HP = skeleton->max_hp;

    printf("\nYou make your way to the prison block, hoping to find supplies or clues.\n");
    getchar();
    printf("\nThe air grows colder as you descend into the lower levels. The metallic smell of blood hangs heavy in the damp air.\n");
    getchar();
    printf("\nAs you push open the rusted iron door, the horrific scene reveals itself - \n");
    printf("dozens of tiny cells line the corridor, their iron bars bent and broken.\n");
    getchar();
    printf("\nMost contain nothing but rotting remains - partial skeletons still chained to walls,\n");
    printf("mounds of flesh crawling with maggots, bloodstains that reach up to the ceiling.\n");
    getchar();
    printf("\nYou step carefully over a severed arm frozen in a clawing position when a gurgling\n");
    printf("noise echoes from the end of the corridor. A shambling figure in tattered prison\n");
    printf("garbs lurches into view, its jaw hanging by a tendon, glowing eyes fixed on you.\n");
    getchar();

    single_combat(p, undead);
    if(p->HP > 0)
    {
        printf("\nAs the first creature falls, bony fingers suddenly grasp your ankle!\n");
        printf("A skeletal prisoner crawls from under a pile of corpses, dragging its\n");
        printf("half-destroyed ribcage across the filthy stone floor.\n");
        getchar();

        single_combat(p, skeleton);
    }

    if(p->HP > 0)
    {
        items Hpotion;
        healing_potion(&Hpotion);
        add_item(p, Hpotion);
        add_item(p, Hpotion);
        printf("\nFound 2 healing potions in the Prison Storage!\n");
        getchar();
        printf("\nFinding no clues as to what exactly transpired here nor any exit-\n");
        getchar();
        printf("\nYou retreat back to the warden's office, the prisoners' final screams\n");
        printf("echoing in your mind.\n");
    }
}

void ritual_site(player *p, enemy *undead, enemy *skeleton)
{
    undead->HP = undead->max_hp;
    skeleton->HP = skeleton->max_hp;

    printf("\nDespite your instincts screaming in disagreement, you decide to head to the ritual site in search of answers.\n");
    getchar();
    printf("\nYou return to the cursed hallway where you first awoke, moving past your initial prison.\n");
    getchar();
    printf("\nThe ritual chamber door lies broken - beyond it, a mountain of corpses forms a grisly landscape.\n");
    getchar();
    printf("\nPlague rats the size of dogs gnaw on remains, their eyes glowing red as they scatter at your approach.\n");
    getchar();
    printf("\nAt the room's center stands a black marble altar carved with Belial's visage - a horned demon with six eyes weeping blood.\n");
    getchar();
    printf("\nA corpse embedded in the wall suddenly tears free, rotten flesh dripping from its bones!\n");
    getchar();

    single_combat(p, undead);

    if(p->HP > 0)
    {
        undead->HP = undead->max_hp;
        printf("\nFrom the pile of bodies emerges another wretch, still clutching its prison chains!\n");
        getchar();
        single_combat(p, undead);
    }

    if(p->HP > 0)
    {
        skeleton->HP = skeleton->max_hp;
        printf("\nOne of the coffins behind the altar shakes violently as skeletal hands burst from its surface, wielding rusted sacrificial knives!\n");
        getchar();
        single_combat(p, skeleton);
    }

    if(p->HP > 0)
    {
        items Hpotion, Mpotion;
        healing_potion(&Hpotion);
        magic_potion(&Mpotion);
        add_item(p, Hpotion);
        add_item(p, Mpotion);
        printf("\nFound a healing potion and magic potion in the pile of bodies!\n");
        getchar();
        printf("\nBeyond the altar, a massive obsidian door pulses with dark energy.\n");
        getchar();
        printf("\nAncient runes rewrite themselves as you watch - whatever magic seals this\n");
        printf("is far beyond your power to break. You retreat back to the warden's office before the corruption takes hold.\n");
        getchar();
    }
}

void save_character(player *p)
{
    FILE *savefile = fopen("saved_character.dat", "wb");
    if (savefile == NULL)
    {
        printf("Error saving game!\n");
        return;
    }

    fwrite(p, sizeof(player), 1, savefile);

    if(savefile != NULL)
    {
        printf("Game saved successfully!\n");
    }

    fclose(savefile);

}

void load_character(player *p)
{
    FILE *savefile = fopen("saved_character.dat", "rb");
    if (savefile == NULL)
    {
        printf("No saved game found!\n");
        return;
    }

    fread(p, sizeof(player), 1, savefile);

    fclose(savefile);
}

