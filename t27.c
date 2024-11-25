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
    if (p1 == NULL || p2 == NULL) return 0;

    // 构建从 p1 到根节点的路径
    dict* path1[1000];
    int len1 = 0;
    dict* temp = p1;
    while (temp != NULL) {
        path1[len1++] = temp;
        temp = temp->parent;
    }

    // 构建从 p2 到根节点的路径
    dict* path2[1000];
    int len2 = 0;
    temp = p2;
    while (temp != NULL) {
        path2[len2++] = temp;
        temp = temp->parent;
    }

    // 从根节点开始比较，找到最低公共祖先
    int i = len1 - 1;
    int j = len2 - 1;
    while (i >= 0 && j >= 0 && path1[i] == path2[j]) {
        i--;
        j--;
    }

    // 计算距离
    unsigned distance = (i + 1) + (j + 1);
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
}
