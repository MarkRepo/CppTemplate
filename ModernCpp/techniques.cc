//
// Created by markfqwu on 2020-03-13.
//
#include "gtest/gtest.h"
#include "ModernCpp/techniques.h"

using namespace modern::tech;

TEST(TechTest, Member){
    Member<int> a{5, "abc"};
    Member<float> f{1.1, 2.2};
    a.echo();
    f.echo();

    Member<char> c{6, "def"};
    c.echo();

    MemberFunc<char, double> general;
    MemberFunc<int, float> partial;
    general.Func();
    partial.Func();
}

TEST(TechTest, policy){
    MyWidgetMgr mgr;
    mgr.Create();
    MyWidgetMgr2 mgr2;
    mgr2.Create();
    mgr2.DoSomething();
    MyWidgetMgr3 mgr3;
    mgr3.Create();
}

TEST(TechTest, policy_destruct){
    MyWidgetMgr22 * pMgr = new MyWidgetMgr22;
    OpNewCreator2<Widget> * pNew = pMgr;
    //delete pNew;
    delete pMgr;
}

TEST(TechTest, partial_initialize){
    WidgetManager4<PrototypeCreator> mgr;
    Widget* w = new Widget;
    mgr.SwitchObj(w);

    WidgetManager4<OpNewCreator> mgr2;
    //mgr2.SwitchObj(w);

    WidgetManager4<MallocCreator> mgr3;
}

TEST(TechTest, smartPtr){

    cout << "------------Start SmartPtr Test-------------" << endl;
    try{
        WidgetPtr pWidget(new Widget);
        WidgetPtr pWidget2(nullptr);
        SafeWidgetPtr pWidget3(nullptr);
        pWidget->echo();
        pWidget2->echo();
        pWidget3->echo();
    }catch(EnforceNotNull<Widget>::EnforceNotNullException& e){
        cout << e.what() << endl;
    }
    catch(std::exception& e){
        cout << e.what() << endl;
    }
    cout << "--------------End SmartPtr Test---------------" << endl;
}

TEST(TechTest, ssmartptr){
    cout << "------------Start SSmartPtr Test-------------" << endl;
    try{
        SWidgetPtr pw1(new Widget);
        SWidgetPtr pw2(nullptr);
        SSafeWidgetPtr pw3(nullptr  );
        pw1->echo();
        pw2->echo();
        pw3->echo();
    }catch(std::exception& e){
        cout << e.what()<< endl;
    }
    cout << "--------------End SSmartPtr Test---------------" << endl;

}

TEST(TechTest, csmartptr){

    cout << "---------------- policies inter transfer---------------" << endl;
    ECWidgetPtr pwe;
    CWidgetPtr pw1(pwe);
    CSafeWidgetPtr pw2(pwe);
}
