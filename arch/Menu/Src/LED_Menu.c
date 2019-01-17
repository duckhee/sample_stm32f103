#include "LED_Menu.h"

LED_MENU_DEF int LED_main();
LED_MENU_DEF int LED_main_menu();

static unsigned short flag;


LED_MENU_DEF int LED_main()
{

    int key;

    while((key=Led_main_menu()) != 0)
    {
        switch(key)
        {
            case '1':
                printf("No.1\n");
                Led_Red_On();
            break;

            case '2':
                printf("No.2\n");
                Led_Red_Off();
            break;
            
            case '3':
                printf("No.3\n");
                Led_Yellow_On();
            break;

            case '4':
                printf("No.4\n");
                Led_Yellow_Off();
            break;
              
            case '5':
                printf("No.5\n");
                Led_Blue_On();
            break;

            case '6':
                printf("No.6\n");
                Led_Blue_Off();
            break;

            case '7':
                printf("No.7\n");
                Led_Test();
            break;

            case '8':
                printf("No.8\n");
                System_Information();
            break;

            case '9':
                printf("No.9\n");
                Led_All_On();
            break;

            case '0':
                printf("No.0\n");
                Led_All_Off();
            break;

            case 'a':
                printf("No.a\n");
            
            break;

            case 'b':
                printf("No.b\n");
              
            break;

            case 'c':
                printf("No.c\n");
            
            break;

            case 'd':
                printf("No.d\n");
        
            break;

            case 'e':
                printf("No.e\n");
     
            break;

            case 'f':
                printf("No.f\n");

            break;

            case 'g':
                printf("No.g\n");
     
            break;

            case 'h':
                printf("No.h\n");
       
            break;

            case 'i':
                printf("No.i\n");

            break;

            case 'j':
                printf("No.j\n");

            break;

            case 'k':
                printf("No.k\n");

            break;

            case 'q':
                return 0;
        }
    }
    return 0;
}

LED_MENU_DEF int LED_main_menu()
{
    int key;

    printf("\n\n");
    printf("-------------------------------------------------\n");
    printf("                 LED MAIN MENU\n");
    printf("-------------------------------------------------\n");
    printf(" 1. Red On                                         \n");
    printf(" 2. Red Off                                         \n");
    printf(" 3. Yellow On                                         \n");
    printf(" 4. Yellow Off                                         \n");
    printf(" 5. Blue On                                         \n");
    printf(" 6. Blue Off                                         \n");
    printf(" 7. Led Test                                         \n");
    printf(" 8. System_Information                                         \n");
    printf(" 9. Test                                         \n");
    printf(" 0. Test                                         \n");
    printf("-------------------------------------------------\n");
    printf(" q. LED Menu QUIT\n");
    printf("-------------------------------------------------\n");
    printf("\n\n");

    printf("SELECT THE COMMAND NUMBER : ");
    key=get_byte();
    
    return key;

}
