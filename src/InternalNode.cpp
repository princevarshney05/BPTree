#include "InternalNode.hpp"
#include "LeafNode.hpp"
#include "RecordPtr.hpp"

//creates internal node pointed to by tree_ptr or creates a new one
InternalNode::InternalNode(const TreePtr &tree_ptr) : TreeNode(INTERNAL, tree_ptr) {
    this->keys.clear();
    this->tree_pointers.clear();
    if(!is_null(tree_ptr))
        this->load();
}

//max element from tree rooted at this node
Key InternalNode::max() {
    Key max_key = DELETE_MARKER;
    TreeNode* last_tree_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    max_key = last_tree_node->max();
    delete last_tree_node;
    return max_key;
}

//if internal node contains a single child, it is returned
TreePtr InternalNode::single_child_ptr() {
    if(this->size == 1)
        return this->tree_pointers[0];
    return NULL_PTR;
}

//inserts <key, record_ptr> into subtree rooted at this node.
//returns pointer to split node if exists
//TODO: InternalNode::insert_key to be implemented
TreePtr InternalNode::insert_key(const Key &key, const RecordPtr &record_ptr) {
    TreePtr new_tree_ptr = NULL_PTR;
    // cout << "InternalNode::insert_key not implemented" << endl;
    TreePtr child_tree_ptr = NULL_PTR;
    int i;
    for(i=0;i<this->keys.size();i++){
        if(key <= this->keys[i]){
            child_tree_ptr = this->tree_pointers[i];
            break;
        }
    }
    if(child_tree_ptr == NULL_PTR){
        child_tree_ptr = this->tree_pointers[this->tree_pointers.size()-1];
    }
    auto child_node = TreeNode::tree_node_factory(child_tree_ptr);
    TreePtr potential_split_node_ptr = child_node->insert_key(key,record_ptr);
    if(!is_null(potential_split_node_ptr)){
        auto left_child_node = TreeNode::tree_node_factory(child_tree_ptr);
        
        this->keys.insert(this->keys.begin()+i,left_child_node->max());
        this->tree_pointers.insert(this->tree_pointers.begin()+i+1,potential_split_node_ptr);
        this->size += 1;

        if(this->overflows()){
            
            auto new_node = InternalNode(new_tree_ptr);
            new_tree_ptr = new_node.tree_ptr;
            int min_req_child = ceil(FANOUT/2.0);
            
            
            for(int i=min_req_child;i<this->tree_pointers.size();i++){
                new_node.tree_pointers.push_back(this->tree_pointers[i]);
                new_node.size += 1;
            }
            this->tree_pointers.erase(this->tree_pointers.begin()+min_req_child,this->tree_pointers.end());
            this->size -= new_node.size;
            for(int i=min_req_child;i<this->keys.size();i++){
                new_node.keys.push_back(this->keys[i]);
            }
            this->keys.erase(this->keys.begin()+min_req_child-1,this->keys.end());
            new_node.dump();
        }
    }
    this->dump();
    
    return new_tree_ptr;
}



//deletes key from subtree rooted at this if exists
//TODO: InternalNode::delete_key to be implemented
void InternalNode::delete_key(const Key &key) {
    TreePtr new_tree_ptr = NULL_PTR;
    // cout << "InternalNode::delete_key not implemented" << endl;
    TreePtr child_tree_ptr = NULL_PTR;
    int i;
    for(i=0;i<this->keys.size();i++){
        if(key <= this->keys[i]){
            child_tree_ptr = this->tree_pointers[i];
            break;
        }
    }
    if(child_tree_ptr == NULL_PTR){
        child_tree_ptr = this->tree_pointers[this->tree_pointers.size()-1];
    }
    auto child_node = TreeNode::tree_node_factory(child_tree_ptr);
    child_node->delete_key(key);
    if(child_node->underflows()){

        if(child_node->node_type == LEAF){

        
            if(i-1>=0) // left sibling exits
            {
                
                auto left_sibling_ptr = this->tree_pointers[i-1];
                auto left_sibling_node = LeafNode(left_sibling_ptr);
                auto cur_child_node = LeafNode(child_tree_ptr);
                if(cur_child_node.size+left_sibling_node.size >= 2*MIN_OCCUPANCY) // redistribution possible 
                {
                
                    int min_req = MIN_OCCUPANCY - cur_child_node.size;
                    
                    map<Key,RecordPtr> extras = left_sibling_node.remove_right(min_req);

                    
                    cur_child_node.data_pointers.insert(extras.begin(),extras.end());
                    cur_child_node.size += min_req;
                    this->keys[i-1] = left_sibling_node.max();
                    cur_child_node.dump();
                }
                else{
                    
                    left_sibling_node.data_pointers.insert(cur_child_node.data_pointers.begin(),cur_child_node.data_pointers.end());
                    left_sibling_node.size += cur_child_node.data_pointers.size();
                    this->keys.erase(this->keys.begin()+i-1);
                    this->tree_pointers.erase(this->tree_pointers.begin()+i);
                    this->size -= 1;
                    left_sibling_node.dump();
                    cur_child_node.data_pointers.clear();
                    cur_child_node.dump();
                }
            }
            else // right-sibling exits
            {
                auto right_sibling_ptr = this->tree_pointers[i+1];
                auto right_sibling_node = LeafNode(right_sibling_ptr);
                auto cur_child_node = LeafNode(child_tree_ptr);
                if(cur_child_node.size+right_sibling_node.size >= 2*MIN_OCCUPANCY) // redistribution possible 
                {
                
                    int min_req = MIN_OCCUPANCY - cur_child_node.size;
                    
                    map<Key,RecordPtr> extras = right_sibling_node.remove_left(min_req);

                    
                    cur_child_node.data_pointers.insert(extras.begin(),extras.end());
                    cur_child_node.size += min_req;
                    this->keys[i] = cur_child_node.max();
                    cur_child_node.dump();
                }
                else{
                    
                    right_sibling_node.data_pointers.insert(cur_child_node.data_pointers.begin(),cur_child_node.data_pointers.end());
                    right_sibling_node.size += cur_child_node.data_pointers.size();
                    this->keys.erase(this->keys.begin()+i);
                    this->tree_pointers.erase(this->tree_pointers.begin()+i);
                    this->size -= 1;
                    right_sibling_node.dump();
                    cur_child_node.data_pointers.clear();
                    cur_child_node.dump();
                }
            }
        }
        else{
            
            if(i-1>=0) // left sibling exits
            {
                
                auto left_sibling_ptr = this->tree_pointers[i-1];
                auto left_sibling_node = InternalNode(left_sibling_ptr);
                auto cur_child_node = InternalNode(child_tree_ptr);
                if(cur_child_node.size+left_sibling_node.size >= 2*MIN_OCCUPANCY) // redistribution possible 
                {
                    
                    int min_req = MIN_OCCUPANCY - cur_child_node.size;

                    vector<int> extra_keys(left_sibling_node.keys.end()-min_req,left_sibling_node.keys.end());
                    left_sibling_node.keys.erase(left_sibling_node.keys.end()-min_req,left_sibling_node.keys.end());

                    vector<TreePtr> extra_pointers(left_sibling_node.tree_pointers.end()-min_req,left_sibling_node.tree_pointers.end());
                    left_sibling_node.tree_pointers.erase(left_sibling_node.tree_pointers.end()-min_req,left_sibling_node.tree_pointers.end());

                    left_sibling_node.size -= min_req;

                    
                    extra_keys.push_back(this->keys[i-1]);
                    this->keys[i-1] = extra_keys[0];
                    extra_keys.erase(extra_keys.begin());

                    cur_child_node.keys.insert(cur_child_node.keys.begin(),extra_keys.begin(),extra_keys.end());
                    cur_child_node.tree_pointers.insert(cur_child_node.tree_pointers.begin(),extra_pointers.begin(),extra_pointers.end());

                    cur_child_node.size += min_req;

                    cur_child_node.dump();
                    left_sibling_node.dump();
                }
                else{ // merging
                    
                    left_sibling_node.keys.push_back(this->keys[i-1]);
                    left_sibling_node.keys.insert(left_sibling_node.keys.end(),cur_child_node.keys.begin(),cur_child_node.keys.end());
                    left_sibling_node.tree_pointers.insert(left_sibling_node.tree_pointers.end(),cur_child_node.tree_pointers.begin(),cur_child_node.tree_pointers.end());
                    left_sibling_node.size += cur_child_node.size;
                    this->keys.erase(this->keys.begin()+i-1);
                    this->tree_pointers.erase(this->tree_pointers.begin()+i);
                    this->size -= 1;
                    left_sibling_node.dump();
                    cur_child_node.keys.clear();
                    cur_child_node.tree_pointers.clear();
                    cur_child_node.dump();
                }
            }
            else // right-sibling exits
            {
                auto right_sibling_ptr = this->tree_pointers[i+1];
                auto right_sibling_node = InternalNode(right_sibling_ptr);
                auto cur_child_node = InternalNode(child_tree_ptr);
                if(cur_child_node.size+right_sibling_node.size >= 2*MIN_OCCUPANCY) // redistribution possible 
                {
                
                    
                    int min_req = MIN_OCCUPANCY - cur_child_node.size;

                    vector<int> extra_keys(right_sibling_node.keys.begin(),right_sibling_node.keys.begin()+min_req);
                    right_sibling_node.keys.erase(right_sibling_node.keys.begin(),right_sibling_node.keys.begin()+min_req);

                    vector<TreePtr> extra_pointers(right_sibling_node.tree_pointers.begin(),right_sibling_node.tree_pointers.begin()+min_req);
                    right_sibling_node.tree_pointers.erase(right_sibling_node.tree_pointers.begin(),right_sibling_node.tree_pointers.begin()+min_req);

                    right_sibling_node.size -= min_req;

                    cur_child_node.keys.push_back(this->keys[i]);
                    this->keys[i] = extra_keys[extra_keys.size()-1];
                    extra_keys.pop_back();

                    cur_child_node.keys.insert(cur_child_node.keys.end(),extra_keys.begin(),extra_keys.end());
                    cur_child_node.tree_pointers.insert(cur_child_node.tree_pointers.end(),extra_pointers.begin(),extra_pointers.end());

                    cur_child_node.size += min_req;

                    cur_child_node.dump();
                    right_sibling_node.dump();
                }
                else{
                    
                    right_sibling_node.keys.insert(right_sibling_node.keys.begin(),this->keys[i]);
                    right_sibling_node.keys.insert(right_sibling_node.keys.begin(),cur_child_node.keys.begin(),cur_child_node.keys.end());
                    right_sibling_node.tree_pointers.insert(right_sibling_node.tree_pointers.begin(),cur_child_node.tree_pointers.begin(),cur_child_node.tree_pointers.end());
                    right_sibling_node.size += cur_child_node.size;
                    this->keys.erase(this->keys.begin()+i);
                    this->tree_pointers.erase(this->tree_pointers.begin()+i);
                    this->size -= 1;
                    right_sibling_node.dump();
                    cur_child_node.keys.clear();
                    cur_child_node.tree_pointers.clear();
                    cur_child_node.dump();
                }
            }
        }

    }

    this->dump();
}

//runs range query on subtree rooted at this node
void InternalNode::range(ostream &os, const Key &min_key, const Key &max_key) const {
    BLOCK_ACCESSES++;
    for(int i = 0; i < this->size - 1; i++){
        if(min_key <= this->keys[i]){
            auto* child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
            child_node->range(os, min_key, max_key);
            delete child_node;
            return;
        }
    }
    auto* child_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    child_node->range(os, min_key, max_key);
    delete child_node;
}

//exports node - used for grading
void InternalNode::export_node(ostream &os) {
    TreeNode::export_node(os);
    for(int i = 0; i < this->size - 1; i++)
        os << this->keys[i] << " ";
    os << endl;
    for(int i = 0; i < this->size; i++){
        auto child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        child_node->export_node(os);
        delete child_node;
    }
}

//writes subtree rooted at this node as a mermaid chart
void InternalNode::chart(ostream &os) {
    string chart_node = this->tree_ptr + "[" + this->tree_ptr + BREAK;
    chart_node += "size: " + to_string(this->size) + BREAK;
    chart_node += "]";
    os << chart_node << endl;

    for(int i = 0; i < this->size; i++) {
        auto tree_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        tree_node->chart(os);
        delete tree_node;
        string link = this->tree_ptr + "-->|";

        if(i == 0)
            link += "x <= " + to_string(this->keys[i]);
        else if (i == this->size - 1) {
            link += to_string(this->keys[i-1]) + " < x";
        } else {
            link += to_string(this->keys[i-1]) + " < x <= " + to_string(this->keys[i]);
        }
        link += "|" + this->tree_pointers[i];
        os << link << endl;
    }
}

ostream& InternalNode::write(ostream &os) const {
    TreeNode::write(os);
    for(int i = 0; i < this->size - 1; i++){
        if(&os == &cout)
            os << "\nP" << i+1 << ": ";
        os << this->tree_pointers[i] << " ";
        if(&os == &cout)
            os << "\nK" << i+1 << ": ";
        os << this->keys[i] << " ";
    }
    if(&os == &cout)
        os << "\nP" << this->size << ": ";
    os << this->tree_pointers[this->size - 1];
    return os;
}

istream& InternalNode::read(istream& is){
    TreeNode::read(is);
    this->keys.assign(this->size - 1, DELETE_MARKER);
    this->tree_pointers.assign(this->size, NULL_PTR);
    for(int i = 0; i < this->size - 1; i++){
        if(&is == &cin)
            cout << "P" << i+1 << ": ";
        is >> this->tree_pointers[i];
        if(&is == &cin)
            cout << "K" << i+1 << ": ";
        is >> this->keys[i];
    }
    if(&is == &cin)
        cout << "P" << this->size;
    is >> this->tree_pointers[this->size - 1];
    return is;
}
