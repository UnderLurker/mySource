//
// Created by 常笑男 on 2024/1/7.
//

#ifndef MAIN_RBTREE_H
#define MAIN_RBTREE_H
#include "Util.h"

NAME_SPACE_START(myUtil)

enum NodeColor{
    RED=0,
    BLACK
};

template <typename keyType, typename valueType, typename Compare = less<keyType>>
class RBNode {
    keyType k{keyType()};
    valueType v{valueType()};
    using Node = RBNode<keyType, valueType, Compare>;

public:
    RBNode() {}
    RBNode(const keyType &key, const valueType &value) : k(key), v(value) {}
    RBNode(const pair<keyType,valueType>& val) : k(val.first), v(val.second) {}
    RBNode(const Node &node) = delete;
    RBNode(const Node &&node) = delete;
    Node& operator=(const Node &) = delete;
    void setLeftChild(const Node *node) {this->lchild = node;}
    Node* getLeftChild() const {return lchild;}
    Node* getRightChild() const {return rchild;}
    void setRightChild(const Node *node) {this->rchild = node;}
    Node* leftRotate() {
        if (this->rchild == nullptr) return nullptr;
        else if(this->rchild->rchild == nullptr){
            Node* next=this->rchild;
            next->parent=this->parent;
            this->parent=next;
            this->rchild=next->lchild;
            next->lchild=this;
            return nullptr;
        }
        else{
            Node *nextRoot=this->rchild;
            if(nextRoot->lchild!=nullptr)
                nextRoot->lchild->parent=this;
            nextRoot->parent=this->parent;
            this->parent=nextRoot;
            this->rchild=nextRoot->lchild;
            nextRoot->lchild=this;
            if(nextRoot->parent==nullptr) return nextRoot;
            if(nextRoot->parent->lchild==this) nextRoot->parent->lchild=nextRoot;
            else nextRoot->parent->rchild=nextRoot;
            return nullptr;
        }
    }
    Node* rightRotate() {
        if (this->lchild == nullptr)  return nullptr;
        else if(this->lchild->lchild == nullptr){
            Node* next=this->lchild;
            next->parent=this->parent;
            this->parent=next;
            this->lchild=next->rchild;
            next->rchild=this;
            return nullptr;
        }
        else{
            Node *nextRoot=this->lchild;
            if(nextRoot->rchild!=nullptr)
                nextRoot->rchild->parent=this;
            nextRoot->parent=this->parent;
            this->parent=nextRoot;
            this->lchild=nextRoot->rchild;
            nextRoot->rchild=this;
            if(nextRoot->parent==nullptr) return nextRoot;
            if(nextRoot->parent->lchild==this) nextRoot->parent->lchild=nextRoot;
            else nextRoot->parent->rchild=nextRoot;
            return nullptr;
        }
    }
    void setValue(const valueType& val) { v=val; }
    valueType getValue() const { return v; }
    const keyType& getKey() const { return k;}
    void reversalColor() { color=color==RED?BLACK:RED; }
    //true为左孩子，否则为右孩子
    bool getLeftOrRight(){ return (void*)(parent->lchild)==(void*)this;}
    Node* balance(){
        //共三种情况
        Node* res=nullptr;
        Node* grandFather=this->parent->parent;
        Node* uncle=grandFather->lchild!=this?grandFather->rchild:grandFather->lchild;
        if(grandFather->lchild==nullptr||grandFather->rchild==nullptr)
            uncle=nullptr;
        if(grandFather->color==BLACK&&uncle!=nullptr&&uncle->color==RED){
            grandFather->reversalColor();
            this->parent->reversalColor();
            uncle->reversalColor();
            if(grandFather->parent==nullptr)
                grandFather->reversalColor();
            else if(grandFather->parent->color==RED)
                return grandFather->balance();
        }
        else if(getLeftOrRight()==parent->getLeftOrRight()){
            if(getLeftOrRight()){
                res=grandFather->rightRotate();
                grandFather->reversalColor();
                grandFather->parent->reversalColor();
            }
            else{
                res=grandFather->leftRotate();
                grandFather->reversalColor();
                grandFather->parent->reversalColor();
            }
        }
        else{
            if(getLeftOrRight()&&!parent->getLeftOrRight()){
                this->parent->rightRotate();
                res=this->parent->leftRotate();
                this->reversalColor();
                this->lchild->reversalColor();
            }
            else if(!getLeftOrRight()&&parent->getLeftOrRight()){
                this->parent->leftRotate();
                res=this->parent->rightRotate();
                this->reversalColor();
                this->rchild->reversalColor();
            }
        }
        return res;
    }
    ~RBNode() {
        if (lchild!=nullptr)
            delete lchild;
        if (rchild!=nullptr)
            delete rchild;
    }
public:
    Node *parent{nullptr};
    Node *lchild{nullptr};
    Node *rchild{nullptr};
    NodeColor color{RED};
};

template <typename keyType, typename valueType, typename Compare = less<keyType>>
class RBTree {
    using Node = RBNode<keyType, valueType, Compare>;
    Node *root{nullptr};

public:
    ~RBTree() {
        if (root)
            delete root;
    }
    Node* find(const keyType& key){
        if(root==nullptr) return root;
        Node* t=root;
        while(t!=nullptr){
            if(t->getKey()==key) return t;
            else if(Compare{}(t->getKey(),key)){
                t=t->lchild;
            }
            else{
                t=t->rchild;
            }
        }
        return nullptr;
    }
    void insert(const keyType& key,const valueType& value){
        if(root==nullptr){
            root=new Node(key,value);
            root->color=BLACK;
            return;
        }
        Node *node=new Node(key,value);
        Node *t=root,*pre=nullptr;
        while(t!=nullptr){
            pre=t;
            if(t->getKey()==key){
                t->setValue(value);
                return;
            }
            else if(Compare{}(t->getKey(),key)) t=t->lchild;
            else t=t->rchild;
        }
        if(Compare{}(pre->getKey(),key)) pre->lchild=node;
        else pre->rchild=node;
        node->parent=pre;
        if(pre->color==BLACK) return;
        else if(node->parent->parent!=nullptr){
            Node* temp=node->balance();
            if(temp!=nullptr){
                root=temp;
            }
        }
    }
    void insert(const pair<keyType,valueType>& pair){
        insert(pair.first,pair.second);
    }
    void print(){
        Node* t=root;
        queue<Node*> s;
        s.push(t);
        while(!s.empty()){
            int size=s.size();
            for(int i=0;i<size;i++){
                Node* a=s.front();
                s.pop();
                cout<<(a->color==RED?"red":"black")<<" key:"<<a->getKey()<<" value:"<<a->getValue()<<"\t";
                if(a->lchild!=nullptr) s.push(a->lchild);
                if(a->rchild!=nullptr) s.push(a->rchild);
            }
            cout<<endl;
        }
        cout<<endl;
    }
};

NAME_SPACE_END();

#endif // MAIN_RBTREE_H
