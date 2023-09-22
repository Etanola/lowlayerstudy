#include<stdbool.h>
#include<stdlib.h>
#include<stdio.h>
#include"9cc.h"

void gen_lval(Node *node) {
    if (node->kind != ND_LVAR)
        error("代入の左辺値が変数ではありません");
    
    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->offset);
    printf("    push rax\n");
}

static int count(void) {
    static int i = 1;
    return i++;
}

void gen(Node *node) {
    switch(node->kind) {
        case ND_NUM: {
            printf("    push %d\n", node->val);
            return;
        }
        case ND_LVAR: {
            gen_lval(node);
            printf("    pop rax\n");
            printf("    mov rax, [rax]\n");
            printf("    push rax\n");
            return;
        }
        case ND_ASSIGN: {
            gen_lval(node->lhs);
            gen(node->rhs);

            printf("    pop rdi\n");
            printf("    pop rax\n");
            printf("    mov [rax], rdi\n");
            printf("    push rdi\n");
            return;
        }
        case ND_RETURN: {
            gen(node->lhs);
            printf("    pop rax\n");
            printf("    mov rsp, rbp\n");
            printf("    pop rbp\n");
            printf("    ret\n");
            return;
        }
        case ND_IF: {
            int c = count();
            gen(node->cond);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je  .Lelse%d\n", c);
            gen(node->then);
            printf("    jmp .Lend%d\n", c);
            printf(".Lelse%d:\n", c);
            if (node->els != NULL)
                gen(node->els);
            printf(".Lend%d:\n", c);
            return;
        }
        case ND_WHILE: {
            int c = count();
            printf(".Lbegin%d:\n", c);
            gen(node->cond);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je  .Lend%d\n", c);
            gen(node->then);
            printf("    jmp .Lbegin%d\n", c);
            printf(".Lend%d:\n", c);
            return;
        }
        case ND_FOR: {
            int c = count();
            if (node->init != NULL)
                gen(node->init);
            printf(".Lbegin%d:\n", c);
            if (node->cond != NULL)
                gen(node->cond);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je  .Lend%d\n", c);
            gen(node->then);
            if (node->inc != NULL)
                gen(node->inc);
            printf("    jmp .Lbegin%d\n", c);
            printf(".Lend%d:\n", c);
            return;
        }

        default:
            break;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind) {
    case ND_ADD:
        printf("    add rax, rdi\n");
        break;
    case ND_SUB:
        printf("    sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("    imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("    cqo\n");
        printf("    div rdi\n");
        break;
    case ND_EQ:
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_NE:
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_LT:
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_LE:
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
        break;
    
    default:
        break;
    }

    printf("    push rax\n");
}