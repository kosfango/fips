class BinaryTree
{

	//################################
			private:
	//################################


struct _bintree
{
   	CString data;
	struct bintree * left;
	struct bintree * right;
};

struct _bintree * root;


//============================================= PRIVATE
	void KillWholeTree(struct _bintree * node);
//=============================================


	//################################
			public:
	//################################



//========================================== Constructor
	BinaryTree(void)
//==========================================
	{
		root = NULL;
	};


//========================================== Destructor
	~BinaryTree(void)
//==========================================
	{
	  	if (root!=NULL)
			KillWholeTree(root);
	};


//==========================================
	int AddToTree(CString &newdata, struct _bintree * root);
//==========================================
//  Adds a CString to the Tree
//  Returns 1 if successfull
//          0 if strings already exists


}; // end of class BinaryTree


//========================================================
	void BinaryTree::KillWholeTree(struct _bintree * node)
//========================================================
{

	if (node->left != NULL)
		KillWholeTree(&node->left);

	if (node->right != NULL)
		KillWholeTree(&node->right);

	free(node);

};

//========================================================
	int BinaryTree::AddToTree(CString &newdata, struct _bintree * root)
//========================================================
{


  if (root == NULL) // empty node => AddToTree
  {
	root = (struct _bintree *)malloc(sizeof(struct _bintree));

	root->left=NULL;
	root->right=NULL;

	root->data = newdata;

	return 1;
  }

  if (strcmpi(newdata,root->data) == 0)
	  return 0;

  if (strcmpi(newdata,root->data) < 0)
	  return (AddToTree(newdata,&root->left));
  else
	  return (AddToTree(newdata,&root->right));

};