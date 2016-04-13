/*
	bw_tree.h

	The actual implementation of Bw-tree. 
*/

#ifndef _BW_TREE_H_
#define _BW_TREE_H_

using byte = unsigned char;

class BwTree {
	public:
		BwTree() {}

		/*
			Interface description

			byte* getPID(key request)

			Steps:
				0) Traverse the tree to find the correct page (index nodes and index-delta-nodes)
					a) Traverse the chain of index delta records (one linked to another by PID, so
						we must repeatedly query memory map for physical pointers)
						i) If the correct pointer is found, query memory map to obtain a pointer to that node.
							Continue recursively
						ii) If the correct pointer is not found, one eventually reaches the index node.
					b) Within the index node find a key k_{i+1} such that 
						k_i < request <= k_{i+1}
					Notice that k_i might be virtual (- infty if request is lower then the smallest key). 
					Similarly, k_{i+1} might also be virtual (if request is higher then the largest key). 
					Those edge cases must be taken care of.
				1) Arrive at non-index nodes. This can be one of: update delta node, delete delta node,
					insert delta node, split delta node.
						i) If a split delta node is found and to find request one must take the side 
						pointer to reach the correct page, the search must halt and complete the SMO. 
						For more information, see below.
						ii) If any other delta node gives a PID for the requested key, return this PID.
						Do not continue searching.
						iii) If no delta node corresponds to our key, return the PID of the actual page.
						Obtaining the data corresponding to the request key now will involve a binary
						search on the contents of the page itself.
				NOTICE: If the length of delta chain has exceeding a predetermined length, we must trigger 
				consolidation. That is true for both the index and page delta chains. Details of consolidation
				later. 

			void insert(byte payload)
				
			Steps:
				0) Obtain the key corresponding to payload. This would be a hashfunction, probably within
				the mem_map
				1) Traverse the tree to find the correct page, just as above. There is pretty much no
				difference in this regard
				2) Create (ask from mem_manager) for a delta-insert record. Propagate will all information.
				This record now points to the page PID we obtained from the end of the former step.
				3) Use CAS to update the memory map. Now, the PID for the page returned from step 1 should
				point to the delta record. 
			NOTICE: CAS can fail and the failure case is not well described in the whole paper. I presume we can 
			just restart the transaction. The only CAS failure case discussed is that happening during consolidation.

			// I'm not positive how to write even invoke this (?)
			void consolidate()

			Consolidation is triggered when the length of a delta chain exceeds predetermined length. It is triggered 
			and carried out by any thread (Accessing, updating etc.). The consolidation means creating a new 
			node/page and updating it with all the changes from delta nodes directly preceeding the page. Then, the
			newly created page is installed into memory map using CAS. On failure of CAS, just abandon this task.
			This means that the original task that the thread was on carries is still executed.

			void finishSMO()

			Since we are only implementing splitting and not merging, finishing SMO means finishing a split. In this
			scenario, one must create an index-delta node, propagate it with information and use CAS to install it in
			place of the current parent in memory map. This delta record will point to the current parent.

			CAS:
			GCC has __sync_val_compare_and_swap() built in. I think we could use it. Better ideas?
		*/

	private:
		// the root is actualy just an index node. see node.h and index_node.h.
		Node* root_;

};

#endif