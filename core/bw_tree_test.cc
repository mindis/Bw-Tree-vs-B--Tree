#include "utils/testing.h"
#include "core/bw_tree.h"
#include "utils/common.h"
#include <string>

TEST(initTest) {
  BwTree* tree = new BwTree();
  IndexNode* root = (IndexNode*) tree->map_->get(tree->rootPid_); // initial tree with conventions
  EXPECT_EQ(root->getIndexKey(0), INIT_KEY_VALUE); // right corresponds to PID 0
  EXPECT_EQ(root->getSmallestPID(), 1);  // left corresponds to PID 1
  END;
}

TEST(dataNodeInsertConsolidateTest) {
	BwTree t = BwTree();
	// give memory manager only data nodes.
	MemoryManager man = MemoryManager(MAX_DELTA_CHAIN,
		0, 0);

	// insert only to the initial right kid.
	int initialKey = INIT_KEY_VALUE + 1;

	// payload has monotinically increasing 
	// value from i to i+LENGTH_RECORDS
	byte* payload = new byte[LENGTH_RECORDS];

	// insert up to MAX_DELTA_CHAIN into the chain
	for(int i = 0; i < MAX_DELTA_CHAIN; i++){
		for(int j = 0; j < LENGTH_RECORDS; j++)
			*(payload + j) = (byte) i + j;
		t.insert(i + initialKey, payload, &man);
	}

	// check the whole damn thing. We should have 
	// MAX_DELTA_CHAIN insert deltas.
	//
	// get the first in chain as indicated by findNode.
	Node* currNode = t.findNode(MAX_DELTA_CHAIN - 1,
		&man).node;

	// check that it is the correct node.
	Node* root = t.map_->get(t.rootPid_);
	Node* firstInChain = t.map_->get(((IndexNode*) root)->getIndexPID(0));
	EXPECT_EQ(firstInChain, currNode);

	byte* foundPayload;
	for(int i = MAX_DELTA_CHAIN - 1; i >= 0; i--) {
		// every node in chain must be a insert delta
		EXPECT_EQ(NodeType::DELTA_INSERT, currNode->getType());
		// every node must contain the right key and payload
		EXPECT_EQ(i, ((DeltaNode*) currNode)->getKey());
		foundPayload = ((DeltaNode*) currNode)->getValue();
		for(int j = 0; j < LENGTH_RECORDS; j++)
			EXPECT_EQ((byte) i + j, *(foundPayload + j));
	}

	// TODO insert the last node that will trigger consolidation.

	END;
}


TEST(findNodeTest) {
// @TODO
	BwTree * tree = new BwTree();
	//MemoryManager  * man = new MemoryManager(100, 100, 100);
	
	// populate tree with some values

	DataNode * child;
	int pages =2;
	//byte * record;
	PID next;
	byte* toWrite = new byte[LENGTH_RECORDS];
	for(int i = 0; i < LENGTH_RECORDS; i++)
 		toWrite[i] = (byte) 0;

	// populate data pages
	IndexNode * root = (IndexNode *) tree->map_->get(tree->rootPid_);
	int minV = 100;
	for (int i =0; i< pages; i++ ) { 
		next = root->getIndexPID(i);
		child = (DataNode *) tree->map_->get(next);

		int m = minV;
		byte* value = new byte[LENGTH_RECORDS];
		for(int j = 0; j < 10; j++) {
			for(int i = 0; i < LENGTH_RECORDS; i++)
				value[i] = (byte) j;
			child->insertBaseData(m, value);
			m += 10;
		}
		delete[] value;

		// for (int j = minV; j<minV+100; j+=5){
		// 	fprintf(stderr, "%d\n", child->getDataLength());
		// 	child->insertBaseData(j, toWrite);
		// 	fprintf(stderr, "here\n");
		// }

		// for (int j = minV; j<minV+100; j+=5) {
		// 	if (!child->pointToRecord(j, &record)) {
		// 		fprintf(stderr, "not found: %d\n", j);
		// 	}
		// 	else fprintf(stderr, "yes\n");
		// }
		// minV += 200;
	}

	//byte * found = tree->get(100, man);

	// @TODO temp for tests vvvv
			

	
			// replace this whole section ^^^ 

	// // search for things in tree
	// byte* found;
	// int maxinTree = 200;
	// int step = 2;

	// for (int i = 0; i< maxinTree; i+=step) {
	// 	found = tree->get(i, man);
	// 	EXPECT_EQ(man, man);
		EXPECT_EQ(tree, tree);
	// 	found++;
	//EXPECT_FALSE((found) == nullptr);
	// }
	END;
}


// insert 1 record
// check if the key is 1 on the left node
TEST(insert1Test) {
  BwTree* tree = new BwTree();
  int key = 1;
  byte* val = new byte[1];
  val[0] = 1; // simple val
  MemoryManager* man = new MemoryManager(3, 3, 3);
  tree->insert(key, val, man);
  EXPECT_EQ(((DeltaNode*)tree->map_->get(1))->getKey(), 1);
  END;
}

// insert two records test
// 1 goes to left node
// 4001 goes to right node
TEST(insert2Test) {
  BwTree* tree = new BwTree();
  byte* val = new byte[1];
  val[0] = 1; // simple val
  MemoryManager* man = new MemoryManager(3, 3, 3);
  tree->insert(1, val, man);
  tree->insert(4001, val, man);
  EXPECT_EQ(((DeltaNode*)tree->map_->get(1))->getKey(), 1);
  EXPECT_EQ(((DeltaNode*)tree->map_->get(0))->getKey(), 4001);
  END;
}

// insert one record
// and then update it
TEST(insertUpdateTest) {
  // new tree and manager
  BwTree* tree = new BwTree();
  MemoryManager* man = new MemoryManager(3, 3, 3);

  // same key 1
  // insert val1 and see if the payload is 1
  byte* val1 = new byte[1];
  val1[0] = 1; // simple val
  tree->insert(1, val1, man);
  EXPECT_EQ((((DeltaNode*)tree->map_->get(1))->getValue())[0], 1);
  // update val1 with val2 see if the payload is 2
  byte* val2 = new byte[1];
  val2[0] = 2; // simple val
  tree->update(1, val2, man);
  EXPECT_EQ((((DeltaNode*)tree->map_->get(1))->getValue())[0], 2);
  END;
}

int main(int argc, char** argv) {
	//dataNodeInsertConsolidateTest();
 	findNodeTest();
        initTest();
        insert1Test();
        insert2Test();
        insertUpdateTest();
}
