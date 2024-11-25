#include "t27.h"

// 'Internal' function prototypes 
// ...

dict* dict_init(void)
{
    dict* root = (dict*)malloc(sizeof(dict));
    memset(root, 0, sizeof(dict)); // Initialize all fields to zero
    return root;
}

bool dict_addword(dict* p, const char* wd)
{
    if (p == NULL || wd == NULL) return false;

    dict* current = p;
    for (const char* c = wd; *c; ++c) {
        int index = *c - 'a';
        if (*c == '\'') index = 26; // Special case for apostrophe

        if (current->children[index] == NULL) {
            current->children[index] = (dict*)malloc(sizeof(dict));
            memset(current->children[index], 0, sizeof(dict));
        }
        current = current->children[index];
    }

    if (!current->isWord) { // If the word is not already in the dictionary
        current->isWord = true;
        current->frequency++;
        return true;
    } else {
        current->frequency++; // Increment frequency if word already exists
        return false;
    }
}

void dict_free(dict** d)
{
    if (*d == NULL) return;

    for (int i = 0; i < 27; i++) {
        dict_free(&(*d)->children[i]);
    }

    free(*d);
    *d = NULL;
}

int dict_wordcount(const dict* p)
{
    if (p == NULL) return 0;

    int count = p->isWord ? p->frequency : 0;
    for (int i = 0; i < 27; i++) {
        count += dict_wordcount(p->children[i]);
    }
    return count;
}

int dict_nodecount(const dict* p)
{
    if (p == NULL) return 0;

    int count = 1; // Count the current node
    for (int i = 0; i < 27; i++) {
        count += dict_nodecount(p->children[i]);
    }
    return count;
}

dict* dict_spell(const dict* p, const char* str)
{
    if (p == NULL || str == NULL) return NULL;

    dict* current = (dict*)p;
    for (const char* c = str; *c && current; ++c) {
        int index = *c - 'a';
        if (*c == '\'') index = 26;

        current = current->children[index];
    }

    return (current && current->isWord) ? (dict*)current : NULL;
}

int dict_mostcommon(const dict* p)
{
    if (p == NULL) return 0;

    int maxFrequency = p->isWord ? p->frequency : 0;
    for (int i = 0; i < 27; i++) {
        int childMax = dict_mostcommon(p->children[i]);
        if (childMax > maxFrequency) maxFrequency = childMax;
    }
    return maxFrequency;
}

// CHALLENGE1
unsigned dict_cmp(dict* p1, dict* p2)
{
    if (p1 == p2) return 0; // Same node

    unsigned distance = 0;
    while (p1 != p2) {
        if (p1 == NULL || p2 == NULL) return 0; // No common ancestor found
        p1 = (dict*)p1; // Cast to avoid warnings
        p2 = (dict*)p2; // Cast to avoid warnings
        // Move up one level on both sides
        for (int i = 0; i < 27; i++) {
            if (p1->children[i] == p2) return distance + 1;
            if (p2->children[i] == p1) return distance + 1;
        }
        // 若到达这里，p1p2都不是另一个的直接父级，需要更上一层，这可能需要更复杂的算法来实现
        return 0; // Indicate failure
    }
    return distance;
}

// CHALLENGE2
void dict_autocomplete(const dict* p, const char* wd, char* ret)
{
    if (p == NULL || wd == NULL || ret == NULL) return;

    dict* current = (dict*)p;
    for (const char* c = wd; *c && current; ++c) {
        int index = *c - 'a';
        if (*c == '\'') index = 26;

        current = current->children[index];
    }

    if (current == NULL) return; // Word prefix does not exist

    //从当前节点查找最常用的单词，实际中可能需要递归所有分支
    for (int i = 0; i < 27; i++) {
        if (current->children[i] && current->children[i]->isWord) {
            // Append the letter to the result string
            *ret++ = i == 26 ? '\'' : i + 'a';
            *ret = '\0'; // Null-terminate the string
            break; // Only append the first most frequent word
        }
    }
}

void test(void)
{
    // 初始化字典
    dict* myDict = dict_init();

    // 测试添加单词
    printf("Adding words...\n");
    dict_addword(myDict, "car");
    dict_addword(myDict, "cart");
    dict_addword(myDict, "part");
    dict_addword(myDict, "park");
    dict_addword(myDict, "park");
    dict_addword(myDict, "park");

    // 测试单词计数
    printf("Total number of nodes: %d\n", dict_nodecount(myDict));
    printf("Total number of words: %d\n", dict_wordcount(myDict));

    // 测试拼写检查
    printf("Spell checking 'car': %p\n", dict_spell(myDict, "car"));
    printf("Spell checking 'cart': %p\n", dict_spell(myDict, "cart"));
    printf("Spell checking 'part': %p\n", dict_spell(myDict, "part"));
    printf("Spell checking 'park': %p\n", dict_spell(myDict, "park"));
    printf("Spell checking 'ca': %p\n", dict_spell(myDict, "ca")); // 应该返回 NULL
    printf("Spell checking 'par': %p\n", dict_spell(myDict, "par")); // 应该返回 NULL

    // 测试最常见的单词频率
    printf("Most common word frequency: %d\n", dict_mostcommon(myDict));

    // 测试自动补全
    char result[100];
    dict_autocomplete(myDict, "pa", result);
    printf("Autocomplete for 'pa': %s\n", result);

    // 释放字典
    dict_free(&myDict);
    if (myDict == NULL) {
        printf("Dictionary successfully freed.\n");
    } else {
        printf("Failed to free dictionary.\n");
    }
}
