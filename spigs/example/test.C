void test() {

gSystem->Load("./sPigsDict.so");
TFile *f = TFile::Open("out-test.root");
PigsEvent *e = 0;
t->SetBranchAddress("e",&e);
int nev = t->GetEntries();


for (i=0;i<nev;i++) {
	t->GetEntry(i); 
//	e->Dump(); 
	cout << "Measurement " << i << " \t counts: " << e->totCounts << endl;
}

t->GetEntry(7);
e->spectrum->Draw();



}
