int bpos = path[i]->FindKeyBrut(pData);
if((bpos == path[i]->GetKeysNum()-1)&&(path[i]->GetRight()!=(-1)))
{
    IX_BTNode* bnode = FetchNode(path[i]->GetRight());
    char* key = NULL;
    RC rc = bnode->GetKey(0,(void*&) key);//compare the pData with the smallest key in the right brother
    if (rc != 0)
    {
        return NULL;
    }
    if (bnode->CompareKey(pData, key) >= 0)
    {
        pos = bnode->FindKeyExact(pData);
        path[i] = bnode;
    }
}
