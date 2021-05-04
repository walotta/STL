/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

    template<
            class Key,
            class T,
            class Compare = std::less<Key>
    > class map {
    public:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::map as value_type by typedef.
         */
        typedef pair<const Key, T> value_type;
    private:
        Compare isLess;
        int cnt=0;
        inline bool equal(const Key& a,const Key& b)const{return isLess(a,b)==isLess(b,a);}
        inline bool small_key(const Key& a,const Key& b)const{return isLess(a,b);}
        class node
        {
        public:
            pair<const Key, T>* value=nullptr;
            node* father=nullptr;
            node* left_son=nullptr;
            node* right_son=nullptr;
            node()=default;
            ~node(){if(value!=nullptr)delete value;}
            inline bool isLeft()const{return father->left_son==this;}
            inline bool isRight()const{return father->right_son==this;}
            inline bool isRoot()const{return father->value==nullptr&&father->father==this;}
            inline void turn_right()
            {
                father->left_son=right_son;
                if(right_son!=nullptr)right_son->father=father;
                right_son=father;
                father=father->father;
                if(father->value==nullptr)
                {
                    father->father=this;
                }else
                {
                    if(right_son->isRight())right_son->father->right_son=this;
                    else right_son->father->left_son=this;
                }
                right_son->father=this;
            }
            inline void turn_left()
            {
                father->right_son=left_son;
                if(left_son!=nullptr)left_son->father=father;
                left_son=father;
                father=father->father;
                if(father->value==nullptr)
                {
                    father->father=this;
                }else
                {
                    if(left_son->isRight())left_son->father->right_son=this;
                    else left_son->father->left_son=this;
                }
                left_son->father=this;
            }
            inline void change_with_father()
            {
                if(isRight())turn_left();
                else turn_right();
            }
            inline void move_up()
            {
                if(father->isRoot())
                {
                    change_with_father();
                } else
                {
                    father->change_with_father();
                    change_with_father();
                }
            }
            inline void splay()
            {
                while(!isRoot())move_up();
            }
            inline node* min_son()const
            {
                node* it=const_cast<node*>(this);
                while(it->left_son!=nullptr)it=it->left_son;
                return it;
            }
            inline node* max_son()const
            {
                node* it=const_cast<node*>(this);
                while(it->right_son!=nullptr)it=it->right_son;
                return it;
            }
            inline node* next()const
            {
                if(right_son!=nullptr)return right_son->min_son();
                else
                {
                    node* it=const_cast<node*>(this);
                    while(it->isRight())it=it->father;
                    return it->father;
                }
            }
            inline node* front()const
            {
                if(left_son!=nullptr)return left_son->max_son();
                else
                {
                    node* it=const_cast<node*>(this);
                    while(it->isLeft())it=it->father;
                    return it->father;
                }
            }
        };
        node* root;
        void DFS_copy_tree(node* my_node,node* target_node)
        {
            if(target_node->value!=nullptr)my_node->value=new value_type(*(target_node->value));
            if(target_node->left_son!=nullptr)
            {
                my_node->left_son=new node;
                my_node->left_son->father=my_node;
                DFS_copy_tree(my_node->left_son,target_node->left_son);
            }
            if(target_node->right_son!=nullptr)
            {
                my_node->right_son=new node;
                my_node->right_son->father=my_node;
                DFS_copy_tree(my_node->right_son,target_node->right_son);
            }
        }
        void DFS_delete(node* now)
        {
            if(now==root)return;
            if(now->left_son!=nullptr)DFS_delete(now->left_son);
            if(now->right_son!=nullptr)DFS_delete(now->right_son);
            //if(now->isRight())now->father->right_son=nullptr;
            //else now->father->left_son=nullptr;
            delete now;
        }
        node* DFS_find(const Key &key,node* now)const
        {
            if(now==nullptr||now->value==nullptr)return nullptr;
            if(equal(key,now->value->first))return now;
            else if(small_key(key,now->value->first))return DFS_find(key,now->left_son);
            else return DFS_find(key,now->right_son);
        }
        node* DFS_insert(const Key& key,node* now)
        {
            if(small_key(key,now->value->first))
            {
                if(now->left_son==nullptr)
                {
                    now->left_son=new node;
                    now->left_son->father=now;
                    now->left_son->value=new value_type(key,T());
                    //if(root->left_son==now)root->left_son=now->left_son;
                    return now->left_son;
                }else return DFS_insert(key,now->left_son);
            }else
            {
                if(now->right_son==nullptr)
                {
                    now->right_son=new node;
                    now->right_son->father=now;
                    now->right_son->value=new value_type(key,T());
                    //if(root->right_son==now)root->right_son=now->right_son;
                    return now->right_son;
                }else return DFS_insert(key,now->right_son);
            }
        }
        inline void copy_from_other_tree(const map &other)
        {
            if(other.root->father==other.root)
            {
                cnt=other.cnt;
                root->father=root;
                return;
            }else
            {
                root->father=new node;
                root->father->father=root;
                DFS_copy_tree(root->father,other.root->father);
                root->left_son=root->father->min_son();
                root->right_son=root->father->max_son();
                cnt=other.cnt;
            }
        }
        inline void reset_tree()
        {
            if(root->father==root)return;
            root->left_son=nullptr;
            root->right_son=nullptr;
            DFS_delete(root->father);
            //delete root;
            //root=new node;
            root->father=root;
            cnt=0;
        }
        inline void merge_tree(node* left_root,node* right_root)
        {
            left_root->father->max_son()->splay();
            left_root->father->right_son=right_root->father;
            right_root->father->father=left_root->father;
        }
        inline int tree_count(const Key& key)const
        {
            node* ans=DFS_find(key,root->father);
            if(ans==nullptr)return 0;
            else return 1;
        }
        inline node* tree_find(const Key &key)const
        {
            node* ans=DFS_find(key,root->father);
            if(ans==nullptr)return nullptr;
            ans->splay();
            root->father=ans;
            return ans;
        }
        inline node* tree_insert(const Key& key)
        {
            //
            cnt++;
            if(root->father==root)
            {
                //树为空
                root->father=new node;
                root->left_son=root->father;
                root->right_son=root->father;
                root->father->father=root;
                root->father->value=new value_type(key,T());
                return root->father;
            }else
            {
                node* ans=DFS_insert(key,root->father);
                if(cnt%10==0)ans->splay();
                if(cnt%1800==0)
                {
                    root->left_son->splay();
                    root->right_son->splay();
                    ans->splay();
                }
                if(small_key(ans->value->first,root->left_son->value->first))root->left_son=ans;
                if(small_key(root->right_son->value->first,ans->value->first))root->right_son=ans;
                return ans;
            }
        }
        inline void remove_node(node* to_remove)
        {
            if(to_remove==nullptr)throw index_out_of_bound();
            cnt--;
            to_remove->splay();
            if(to_remove->left_son==nullptr&&to_remove->right_son==nullptr)
            {
                delete to_remove;
                root->father=root;
                root->left_son=nullptr;
                root->right_son=nullptr;
                return;
            }
            if(equal(to_remove->value->first,root->left_son->value->first))root->left_son=to_remove->next();
            if(equal(to_remove->value->first,root->right_son->value->first))root->right_son=to_remove->front();
            if(to_remove->left_son==nullptr)
            {
                root->father=to_remove->right_son;
                to_remove->right_son->father=root;
                delete to_remove;
            }else if(to_remove->right_son==nullptr)
            {
                root->father=to_remove->left_son;
                to_remove->left_son->father=root;
                delete to_remove;
            }else
            {
                //root->father=to_remove;
                node* left_root=new node;
                node* right_root=new node;
                left_root->father=to_remove->left_son;
                to_remove->left_son->father=left_root;
                right_root->father=to_remove->right_son;
                to_remove->right_son->father=right_root;
                merge_tree(left_root,right_root);
                root->father=left_root->father;
                root->father->father=root;
                delete to_remove;
                delete left_root;
                delete right_root;
            }
        }
        inline void tree_remove(const Key& key)
        {
            node* to_remove=DFS_find(key,root->father);
            remove_node(to_remove);
        }
    public:
        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = map.begin(); --it;
         *       or it = map.end(); ++end();
         */
        class const_iterator;
        class iterator {
        private:
            /**
             * add data members
             *   just add whatever you want.
             */
             node* target=nullptr;
             map* belongs=nullptr;
             template<class X,class Y,class Z>
             friend class map;
        public:
            iterator()=default;
            iterator(node* _tar,map* _bel):target(_tar),belongs(_bel){}
            iterator(const iterator &other):target(other.target),belongs(other.belongs) {}
            /**
             * iter++
             */
            iterator operator++(int) {
                if(belongs->cnt==0)throw invalid_iterator();
                if(target==belongs->root)throw invalid_iterator();
                auto tem=*this;
                if(target==belongs->root->right_son)
                    target=belongs->root;
                else
                    target=target->next();
                return tem;
            }
            /**
             * ++iter
             */
            iterator & operator++() {
                if(belongs->cnt==0)throw invalid_iterator();
                if(target==belongs->root)throw invalid_iterator();
                if(target==belongs->root->right_son)
                    target=belongs->root;
                else
                    target=target->next();
                return *this;
            }
            /**
             * iter--
             */
            iterator operator--(int) {
                if(belongs->cnt==0)throw invalid_iterator();
                if(target==belongs->root->left_son)throw invalid_iterator();
                auto tem=*this;
                if(target==belongs->root)
                    target=belongs->root->right_son;
                else
                    target=target->front();
                return tem;
            }
            /**
             * --iter
             */
            iterator & operator--() {
                if(belongs->cnt==0)throw invalid_iterator();
                if(target==belongs->root->left_son)throw invalid_iterator();
                if(target==belongs->root)
                    target=belongs->root->right_son;
                else
                    target=target->front();
                return *this;
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type & operator*() const {
                return *(target->value);
            }
            bool operator==(const iterator &rhs) const {
                return (belongs==rhs.belongs&&target==rhs.target);
            }
            bool operator==(const const_iterator &rhs) const {
                return (belongs==rhs.belongs&&target==rhs.target);
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return (belongs!=rhs.belongs||target!=rhs.target);
            }
            bool operator!=(const const_iterator &rhs) const {
                return (belongs!=rhs.belongs||target!=rhs.target);
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type* operator->() const noexcept {
                return target->value;
            }
        };
        class const_iterator {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        private:
            // data members.
            const node* target=nullptr;
            const map* belongs=nullptr;
            template<class X,class Y,class Z>
            friend class map;
        public:
            const_iterator()=default;
            const_iterator(const node* _tar,const map* _bel):target(_tar),belongs(_bel){}
            const_iterator(const const_iterator &other):target(other.target),belongs(other.belongs){}
            const_iterator(const iterator &other):target(other.target),belongs(other.belongs){}
            const_iterator operator++(int) {
                if(belongs->cnt==0)throw invalid_iterator();
                if(target==belongs->root)throw invalid_iterator();
                auto tem=*this;
                if(target==belongs->root->right_son)
                    target=belongs->root;
                else
                    target=target->next();
                return tem;
            }
            const_iterator & operator++() {
                if(belongs->cnt==0)throw invalid_iterator();
                if(target==belongs->root)throw invalid_iterator();
                if(target==belongs->root->right_son)
                    target=belongs->root;
                else
                    target=target->next();
                return *this;
            }
            const_iterator operator--(int) {
                if(belongs->cnt==0)throw invalid_iterator();
                if(target==belongs->root->left_son)throw invalid_iterator();
                auto tem=*this;
                if(target==belongs->root)
                    target=belongs->root->right_son;
                else
                    target=target->front();
                return tem;
            }
            const_iterator & operator--() {
                if(belongs->cnt==0)throw invalid_iterator();
                if(target==belongs->root->left_son)throw invalid_iterator();
                if(target==belongs->root)
                    target=belongs->root->right_son;
                else
                    target=target->front();
                return *this;
            }
            const value_type & operator*() const {
                return *(target->value);
            }
            bool operator==(const iterator &rhs) const {
                return (belongs==rhs.belongs&&target==rhs.target);
            }
            bool operator==(const const_iterator &rhs) const {
                return (belongs==rhs.belongs&&target==rhs.target);
            }
            bool operator!=(const iterator &rhs) const {
                return (belongs!=rhs.belongs||target!=rhs.target);
            }
            bool operator!=(const const_iterator &rhs) const {
                return (belongs!=rhs.belongs||target!=rhs.target);
            }
            const value_type* operator->() const noexcept {
                return target->value;
            }
        };
        /**
         * two constructors
         */
        map() {
            root=new node;
            root->father=root;
        }
        map(const map &other) {
            root=new node;
            root->father=root;
            copy_from_other_tree(other);
        }
        /**
         * assignment operator
         */
        map & operator=(const map &other) {
            if(this==&other)return *this;
            reset_tree();
            copy_from_other_tree(other);
            return *this;
        }
        /**
         * Destructors
         */
        ~map() {
            reset_tree();
            delete root;
        }
        /**
         *
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        T & at(const Key &key) {
            node* ans=tree_find(key);
            if(ans==nullptr)throw index_out_of_bound();
            return ans->value->second;
        }
        const T & at(const Key &key) const {
            node* ans=tree_find(key);
            if(ans==nullptr)throw index_out_of_bound();
            return ans->value->second;
        }
        /**
         *
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T & operator[](const Key &key) {
            node* ans=tree_find(key);
            if(ans==nullptr)return tree_insert(key)->value->second;
            else return ans->value->second;
        }
        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T & operator[](const Key &key) const {
            node* ans=tree_find(key);
            if(ans==nullptr)throw index_out_of_bound();
            else return ans->value->second;
        }
        /**
         * return a iterator to the beginning
         */
        iterator begin() {
            if(cnt==0)return iterator(root,this);
            return iterator(root->left_son,this);
        }
        const_iterator cbegin() const {
            if(cnt==0)return const_iterator(root,this);
            return const_iterator(root->left_son,this);
        }
        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() {
            return iterator(root,this);
        }
        const_iterator cend() const {
            return const_iterator(root,this);
        }
        /**
         * checks whether the container is empty
         * return true if empty, otherwise false.
         */
        bool empty() const {
            return root->father==root;
        }
        /**
         * returns the number of elements.
         */
        size_t size() const {return cnt;}
        /**
         * clears the contents
         */
        void clear() {
            reset_tree();
        }
        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        pair<iterator, bool> insert(const value_type &value) {
            node* exist=tree_find(value.first);
            if(exist==nullptr)
            {
                node* n=tree_insert(value.first);
                n->value->second=value.second;
                return pair<iterator,bool>(iterator(n,this),true);
            }else return pair<iterator,bool>(iterator(exist,this),false);
        }
        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos) {
            if(pos==end()||pos.belongs!=this)throw invalid_iterator();
            remove_node(pos.target);
        }
        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         * The default method of check the equivalence is !(a < b || b > a)
         */
        size_t count(const Key &key) const {
            return tree_count(key);
        }
        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key){
            node* ans=tree_find(key);
            if(ans==nullptr)return end();
            else return iterator(ans,this);
        }
        const_iterator find(const Key &key) const {
            node* ans=tree_find(key);
            if(ans==nullptr)return cend();
            else return const_iterator(ans,this);
        }

    private:
        int debug_dfs_check(node* now)
        {
            if(now->father==now&&cnt==0)return 0;
            if(now->father==nullptr)return -1;
            auto it=now->value->first;
            int flag=0;
            if(now->right_son!=nullptr)
            {
                if(small_key(now->right_son->value->first,it))return -1;
                if(now->right_son->father!=now)return -1;
                int tem=debug_dfs_check(now->right_son);
                if(tem==-1)return -1;
                flag+=tem;
            }
            if(now->left_son!=nullptr)
            {
                if(small_key(it,now->left_son->value->first))return -1;
                if(now->left_son->father!=now)return -1;
                int tem=debug_dfs_check(now->left_son);
                if(tem==-1)return -1;
                flag+=tem;
            }
            return flag+1;
        }
    public:
        void debug_check_tree(int id=-1)
        {
            printf("\n===== check for cnt = %d =====\n",cnt);
            if(id!=-1)printf("id = %d\n",id);
            if(debug_dfs_check(root->father)==cnt)
                printf("correct\n=======\n");
            else
                printf("error\n"),exit(0);
        }
    };

}

#endif