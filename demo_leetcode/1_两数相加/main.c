//
// Created by longduping on 2019/11/26.
//
/*  给出两个 非空 的链表用来表示两个非负的整数。其中，它们各自的位数是按照 逆序 的方式存储的，并且它们的每个节点只能存储 一位 数字。
    如果，我们将这两个数相加起来，则会返回一个新的链表来表示它们的和。
    您可以假设除了数字 0 之外，这两个数都不会以 0 开头。

    示例：

    输入：(2 -> 4 -> 3) + (5 -> 6 -> 4)
    输出：7 -> 0 -> 8
    原因：342 + 465 = 807

    来源：力扣（LeetCode）
    链接：https://leetcode-cn.com/problems/add-two-numbers
    著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。
 * */


#include <stdlib.h>
#include <stdio.h>

//Definition for singly-linked list.
struct ListNode {
    int val;
    struct ListNode *next;
};


struct ListNode *addTwoNumbers(struct ListNode *l1, struct ListNode *l2)
{
    struct ListNode *res = malloc(sizeof(struct ListNode));
    struct ListNode *p1 = l1, *p2 = l2, *pres = res;
    int carry = 0, x, y;
    while (p1 || p2) {
        x = p1 ? p1->val : 0;
        y = p2 ? p2->val : 0;
        pres->next = malloc(sizeof(struct ListNode));
        pres->next->next = NULL;
        pres->next->val = (x + y + carry) % 10;
        carry = (x + y + carry) / 10;
        pres = pres->next;
        if (p1) p1 = p1->next;
        if (p2) p2 = p2->next;
    }
    if (carry) {
        pres->next = malloc(sizeof(struct ListNode));
        pres->next->val = 1;
    }
    return res->next;
}

int main()
{
    struct ListNode *l1 = malloc(sizeof(struct ListNode));
    l1->val = 1;
//    l1->next = malloc(sizeof(struct ListNode));
//    l1->next->val = 8;
//    l1->next->next = malloc(sizeof(struct ListNode));
//    l1->next->next->val = 6;

    struct ListNode *l2 = malloc(sizeof(struct ListNode));
    l2->val = 9;
    l2->next = malloc(sizeof(struct ListNode));
    l2->next->val = 9;
    l2->next->next = NULL;

    struct ListNode *res = addTwoNumbers(l1, l2);
    while (res) {
        printf("%d ", res->val);
        res = res->next;
    }
    return 0;
}

