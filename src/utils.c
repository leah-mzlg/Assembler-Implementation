#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"


char* skip_whitespaces(char *str) {
    while (isspace((unsigned char)*str)) str++;
    return str;
}

char *trim_whitespaces(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == '\0') return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

int is_valid_numeric_string(char *str) {
    if (str == NULL || *str == '\0') return 0;

    /* skip optional sign */
    if (*str == '-' || *str == '+') {
        str++;
    }

    /* must have at least one digit */
    if (*str == '\0') return 0;

    /* check that all remaining characters are digits */
    while (*str) {
        if (!isdigit((unsigned char)*str)) {
            return 0;
        }
        str++;
    }

    return 1;
}

char *my_strdup(const char *str) {
    char *copy = malloc(strlen(str) + 1);
    if (copy != NULL) {
        strcpy(copy, str);
    }
    return copy;
}


int has_consecutive_commas(const char *str) {
    /* אנחנו מאתחלים את המצב ל-1 (כאילו מצאנו פסיק).
       זה אומר שכל עוד לא פגשנו תו אמיתי (אות/מספר) - המערכת נעולה.
       אם נתקל בפסיק לפני שפגשנו תו אמיתי שיפתח את הנעילה (ויאפס ל-0), זו תהיה שגיאה!
    */
    int expect_operand = 1; 

    while (*str != '\0') {
        if (*str == ',') {
            /* אם הגענו לפסיק והמערכת עדיין מצפה לאופרנד (לפניו), זו שגיאה! */
            if (expect_operand) return 1; 
            
            /* אחרי שפגשנו פסיק תקין, המערכת שוב ננעלת ומצפה לאופרנד הבא */
            expect_operand = 1; 
        } 
        else if (!isspace((unsigned char)*str)) {
            /* ברגע שפגשנו תו חוקי (שהוא לא רווח), הנעילה משתחררת */
            expect_operand = 0; 
        }
        str++;
    }

    /* בדיקה כללית לסוף המחרוזת:
       אם הלולאה הסתיימה וה-expect_operand עדיין שווה 1,
       זה אומר שהתו האחרון (הלא-רווח) בשורה היה פסיק (למשל: "mov r1, r2, ")
    */
    if (expect_operand) {
        return 1;
    }

    return 0;
}
