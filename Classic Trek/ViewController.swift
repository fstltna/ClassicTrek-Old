//
//  ViewController.swift
//  Classic Trek
//
//  Created by Marisa Giancarla on 12/6/15.
//  Copyright © 2015 Pocketfiction. All rights reserved.
//

import UIKit
import AVFoundation

class ViewController: UIViewController {
    @IBOutlet var textView: UITextView!
    @IBAction func MoveButtonAction(sender: AnyObject) {
        
        let userinput = inputField.text
        print(userinput)
        updateTextView(userinput!)
        inputField.text = ""
    }
    @IBOutlet var bottomHeight: NSLayoutConstraint!
    
    @IBOutlet var inputField: UITextField!
    var myArgs = CStringArray(["runIT", "", "", ""])

    var bgMusic:AVAudioPlayer = AVAudioPlayer()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "keyboardWillShow:", name: UIKeyboardWillShowNotification, object: nil)
        
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "keyboardWillHide:", name: UIKeyboardWillHideNotification, object: nil)

        // Do any additional setup after loading the view, typically from a nib.
        let bgMusicURL:NSURL = NSBundle.mainBundle().URLForResource("TrekSoundtrack", withExtension: "mp3")!
        do { bgMusic = try AVAudioPlayer(contentsOfURL: bgMusicURL, fileTypeHint: nil) } catch _ { return print("file not found") }
        bgMusic.numberOfLoops = 1
        bgMusic.prepareToPlay()
        bgMusic.play()

        cmb_main();
        
        NSTimer.scheduledTimerWithTimeInterval(0.1, target: self, selector: "update", userInfo: nil, repeats: true);
        
    }
    
    func keyboardWillShow(notification:NSNotification) {
        adjustingHeight(true, notification: notification)
    }
    
    func keyboardWillHide(notification:NSNotification) {
        adjustingHeight(false, notification: notification)
    }
    
    func adjustingHeight(show:Bool, notification:NSNotification) {
        // 1
        var userInfo = notification.userInfo!
        // 2
        let keyboardFrame:CGRect = (userInfo[UIKeyboardFrameBeginUserInfoKey] as! NSValue).CGRectValue()
        // 3
        let animationDurarion = userInfo[UIKeyboardAnimationDurationUserInfoKey] as! NSTimeInterval
        // 4
        let changeInHeight = (CGRectGetHeight(keyboardFrame)) * (show ? 1 : -1)
        //5
        UIView.animateWithDuration(animationDurarion, animations: { () -> Void in
            self.bottomHeight.constant += changeInHeight
        })
        
    }
    func update(){//display result
        if(cmb_check()>0){
            let resultString = String.fromCString(cmbResult) ;           // The Swift String to be
            self.textView.text = self.textView.text + resultString!  + "\n";
            clear_cmb_result();
        }

    }
    override func viewWillDisappear(animated: Bool) {
        NSNotificationCenter.defaultCenter().removeObserver(self, name: UIKeyboardWillShowNotification, object: nil)
        NSNotificationCenter.defaultCenter().removeObserver(self, name: UIKeyboardWillHideNotification, object: nil)
    }

    
    // Whatever passed in the updateTextView Function will be displayed in the textView
    func updateTextView(inputText: String) -> String {
        
        
        cmb_start();
        
        for u in inputText.utf16{
            if u==0 {
                cmb_add(0);
            }
            if u==1 {
                cmb_add(1);
            }
            if u==2 {
                cmb_add(2);
            }
            if u==3 {
                cmb_add(3);
            }
            if u==4 {
                cmb_add(4);
            }
            if u==5 {
                cmb_add(5);
            }
            if u==6 {
                cmb_add(6);
            }
            if u==7 {
                cmb_add(7);
            }
            if u==8 {
                cmb_add(8);
            }
            if u==9 {
                cmb_add(9);
            }
            
            if u==10 {
                cmb_add(10);
            }
            if u==11 {
                cmb_add(11);
            }
            if u==12 {
                cmb_add(12);
            }
            if u==13 {
                cmb_add(13);
            }
            if u==14 {
                cmb_add(14);
            }
            if u==15 {
                cmb_add(15);
            }
            if u==16 {
                cmb_add(16);
            }
            if u==17 {
                cmb_add(17);
            }
            if u==18 {
                cmb_add(18);
            }
            if u==19 {
                cmb_add(19);
            }
            
            if u==20 {
                cmb_add(20);
            }
            if u==21 {
                cmb_add(21);
            }
            if u==22 {
                cmb_add(22);
            }
            if u==23 {
                cmb_add(23);
            }
            if u==24 {
                cmb_add(24);
            }
            if u==25 {
                cmb_add(25);
            }
            if u==26 {
                cmb_add(26);
            }
            if u==27 {
                cmb_add(27);
            }
            if u==28 {
                cmb_add(28);
            }
            if u==29 {
                cmb_add(29);
            }
            
            if u==30 {
                cmb_add(30);
            }
            if u==31 {
                cmb_add(31);
            }
            if u==32 {
                cmb_add(32);
            }
            if u==33 {
                cmb_add(33);
            }
            if u==34 {
                cmb_add(34);
            }
            if u==35 {
                cmb_add(35);
            }
            if u==36 {
                cmb_add(36);
            }
            if u==37 {
                cmb_add(37);
            }
            if u==38 {
                cmb_add(38);
            }
            if u==39 {
                cmb_add(39);
            }
            
            if u==40 {
                cmb_add(40);
            }
            if u==41 {
                cmb_add(41);
            }
            if u==42 {
                cmb_add(42);
            }
            if u==43 {
                cmb_add(43);
            }
            if u==44 {
                cmb_add(44);
            }
            if u==45 {
                cmb_add(45);
            }
            if u==46 {
                cmb_add(46);
            }
            if u==47 {
                cmb_add(47);
            }
            if u==48 {
                cmb_add(48);
            }
            if u==49 {
                cmb_add(49);
            }
            
            if u==50 {
                cmb_add(50);
            }
            if u==51 {
                cmb_add(51);
            }
            if u==52 {
                cmb_add(52);
            }
            if u==53 {
                cmb_add(53);
            }
            if u==54 {
                cmb_add(54);
            }
            if u==55 {
                cmb_add(55);
            }
            if u==56 {
                cmb_add(56);
            }
            if u==57 {
                cmb_add(57);
            }
            if u==58 {
                cmb_add(58);
            }
            if u==59 {
                cmb_add(59);
            }
            
            if u==60 {
                cmb_add(60);
            }
            if u==61 {
                cmb_add(61);
            }
            if u==62 {
                cmb_add(62);
            }
            if u==63 {
                cmb_add(63);
            }
            if u==64 {
                cmb_add(64);
            }
            if u==65 {
                cmb_add(65);
            }
            if u==66 {
                cmb_add(66);
            }
            if u==67 {
                cmb_add(67);
            }
            if u==68 {
                cmb_add(68);
            }
            if u==69 {
                cmb_add(69);
            }
            
            if u==70 {
                cmb_add(70);
            }
            if u==71 {
                cmb_add(71);
            }
            if u==72 {
                cmb_add(72);
            }
            if u==73 {
                cmb_add(73);
            }
            if u==74 {
                cmb_add(74);
            }
            if u==75 {
                cmb_add(75);
            }
            if u==76 {
                cmb_add(76);
            }
            if u==77 {
                cmb_add(77);
            }
            if u==78 {
                cmb_add(78);
            }
            if u==79 {
                cmb_add(79);
            }
            
            if u==80 {
                cmb_add(80);
            }
            if u==81 {
                cmb_add(81);
            }
            if u==82 {
                cmb_add(82);
            }
            if u==83 {
                cmb_add(83);
            }
            if u==84 {
                cmb_add(84);
            }
            if u==85 {
                cmb_add(85);
            }
            if u==86 {
                cmb_add(86);
            }
            if u==87 {
                cmb_add(87);
            }
            if u==88 {
                cmb_add(88);
            }
            if u==89 {
                cmb_add(89);
            }
            
            if u==90 {
                cmb_add(90);
            }
            if u==91 {
                cmb_add(91);
            }
            if u==92 {
                cmb_add(92);
            }
            if u==93 {
                cmb_add(93);
            }
            if u==94 {
                cmb_add(94);
            }
            if u==95 {
                cmb_add(95);
            }
            if u==96 {
                cmb_add(96);
            }
            if u==97 {
                cmb_add(97);
            }
            if u==98 {
                cmb_add(98);
            }
            if u==99 {
                cmb_add(99);
            }
            
            if u==100 {
                cmb_add(100);
            }
            if u==101 {
                cmb_add(101);
            }
            if u==102 {
                cmb_add(102);
            }
            if u==103 {
                cmb_add(103);
            }
            if u==104 {
                cmb_add(104);
            }
            if u==105 {
                cmb_add(105);
            }
            if u==106 {
                cmb_add(106);
            }
            if u==107 {
                cmb_add(107);
            }
            if u==108 {
                cmb_add(108);
            }
            if u==109 {
                cmb_add(109);
            }
            
            if u==110 {
                cmb_add(110);
            }
            if u==111 {
                cmb_add(111);
            }
            if u==112 {
                cmb_add(112);
            }
            if u==113 {
                cmb_add(113);
            }
            if u==114 {
                cmb_add(114);
            }
            if u==115 {
                cmb_add(115);
            }
            if u==116 {
                cmb_add(116);
            }
            if u==117 {
                cmb_add(117);
            }
            if u==118 {
                cmb_add(118);
            }
            if u==119 {
                cmb_add(119);
            }
            
            if u==120 {
                cmb_add(120);
            }
            if u==121 {
                cmb_add(121);
            }
            if u==122 {
                cmb_add(122);
            }
            if u==123 {
                cmb_add(123);
            }
            if u==124 {
                cmb_add(124);
            }
            if u==125 {
                cmb_add(125);
            }
            if u==126 {
                cmb_add(126);
            }
            if u==127 {
                cmb_add(127);
            }
            if u==128 {
                cmb_add(128);
            }
            if u==129 {
                cmb_add(129);
            }
            
            if u==130 {
                cmb_add(130);
            }
            if u==131 {
                cmb_add(131);
            }
            if u==132 {
                cmb_add(132);
            }
            if u==133 {
                cmb_add(133);
            }
            if u==134 {
                cmb_add(134);
            }
            if u==135 {
                cmb_add(135);
            }
            if u==136 {
                cmb_add(136);
            }
            if u==137 {
                cmb_add(137);
            }
            if u==138 {
                cmb_add(138);
            }
            if u==139 {
                cmb_add(139);
            }
            
            if u==140 {
                cmb_add(140);
            }
            if u==141 {
                cmb_add(141);
            }
            if u==142 {
                cmb_add(142);
            }
            if u==143 {
                cmb_add(143);
            }
            if u==144 {
                cmb_add(144);
            }
            if u==145 {
                cmb_add(145);
            }
            if u==146 {
                cmb_add(146);
            }
            if u==147 {
                cmb_add(147);
            }
            if u==148 {
                cmb_add(148);
            }
            if u==149 {
                cmb_add(149);
            }
            
            if u==150 {
                cmb_add(150);
            }
            if u==151 {
                cmb_add(151);
            }
            if u==152 {
                cmb_add(152);
            }
            if u==153 {
                cmb_add(153);
            }
            if u==154 {
                cmb_add(154);
            }
            if u==155 {
                cmb_add(155);
            }
            if u==156 {
                cmb_add(156);
            }
            if u==157 {
                cmb_add(157);
            }
            if u==158 {
                cmb_add(158);
            }
            if u==159 {
                cmb_add(159);
            }
            
            if u==160 {
                cmb_add(160);
            }
            if u==161 {
                cmb_add(161);
            }
            if u==162 {
                cmb_add(162);
            }
            if u==163 {
                cmb_add(163);
            }
            if u==164 {
                cmb_add(164);
            }
            if u==165 {
                cmb_add(165);
            }
            if u==166 {
                cmb_add(166);
            }
            if u==167 {
                cmb_add(167);
            }
            if u==168 {
                cmb_add(168);
            }
            if u==169 {
                cmb_add(169);
            }
            
            if u==170 {
                cmb_add(170);
            }
            if u==171 {
                cmb_add(171);
            }
            if u==172 {
                cmb_add(172);
            }
            if u==173 {
                cmb_add(173);
            }
            if u==174 {
                cmb_add(174);
            }
            if u==175 {
                cmb_add(175);
            }
            if u==176 {
                cmb_add(176);
            }
            if u==177 {
                cmb_add(177);
            }
            if u==178 {
                cmb_add(178);
            }
            if u==179 {
                cmb_add(179);
            }
            
            if u==180 {
                cmb_add(180);
            }
            if u==181 {
                cmb_add(181);
            }
            if u==182 {
                cmb_add(182);
            }
            if u==183 {
                cmb_add(183);
            }
            if u==184 {
                cmb_add(184);
            }
            if u==185 {
                cmb_add(185);
            }
            if u==186 {
                cmb_add(186);
            }
            if u==187 {
                cmb_add(187);
            }
            if u==188 {
                cmb_add(188);
            }
            if u==189 {
                cmb_add(189);
            }
            
            if u==190 {
                cmb_add(190);
            }
            if u==191 {
                cmb_add(191);
            }
            if u==192 {
                cmb_add(192);
            }
            if u==193 {
                cmb_add(193);
            }
            if u==194 {
                cmb_add(194);
            }
            if u==195 {
                cmb_add(195);
            }
            if u==196 {
                cmb_add(196);
            }
            if u==197 {
                cmb_add(197);
            }
            if u==198 {
                cmb_add(198);
            }
            if u==199 {
                cmb_add(199);
            }
            
            if u==200 {
                cmb_add(200);
            }
            if u==201 {
                cmb_add(201);
            }
            if u==202 {
                cmb_add(202);
            }
            if u==203 {
                cmb_add(203);
            }
            if u==204 {
                cmb_add(204);
            }
            if u==205 {
                cmb_add(205);
            }
            if u==206 {
                cmb_add(206);
            }
            if u==207 {
                cmb_add(207);
            }
            if u==208 {
                cmb_add(208);
            }
            if u==209 {
                cmb_add(209);
            }
            
            if u==210 {
                cmb_add(210);
            }
            if u==211 {
                cmb_add(211);
            }
            if u==212 {
                cmb_add(212);
            }
            if u==213 {
                cmb_add(3);
            }
            if u==214 {
                cmb_add(214);
            }
            if u==215 {
                cmb_add(215);
            }
            if u==216 {
                cmb_add(216);
            }
            if u==217 {
                cmb_add(217);
            }
            if u==218 {
                cmb_add(218);
            }
            if u==219 {
                cmb_add(219);
            }
            
            if u==220 {
                cmb_add(0);
            }
            if u==221 {
                cmb_add(221);
            }
            if u==222 {
                cmb_add(222);
            }
            if u==223 {
                cmb_add(223);
            }
            if u==224 {
                cmb_add(224);
            }
            if u==225 {
                cmb_add(225);
            }
            if u==226 {
                cmb_add(226);
            }
            if u==227 {
                cmb_add(227);
            }
            if u==228 {
                cmb_add(228);
            }
            if u==229 {
                cmb_add(229);
            }
            
            if u==230 {
                cmb_add(230);
            }
            if u==231 {
                cmb_add(231);
            }
            if u==232 {
                cmb_add(232);
            }
            if u==233 {
                cmb_add(233);
            }
            if u==234 {
                cmb_add(234);
            }
            if u==235 {
                cmb_add(235);
            }
            if u==236 {
                cmb_add(236);
            }
            if u==237 {
                cmb_add(237);
            }
            if u==238 {
                cmb_add(238);
            }
            if u==239 {
                cmb_add(239);
            }
            
            if u==240 {
                cmb_add(240);
            }
            if u==241 {
                cmb_add(241);
            }
            if u==242 {
                cmb_add(242);
            }
            if u==243 {
                cmb_add(243);
            }
            if u==244 {
                cmb_add(244);
            }
            if u==245 {
                cmb_add(245);
            }
            if u==246 {
                cmb_add(246);
            }
            if u==247 {
                cmb_add(247);
            }
            if u==248 {
                cmb_add(248);
            }
            if u==249 {
                cmb_add(249);
            }
            
            if u==250 {
                cmb_add(250);
            }
            if u==251 {
                cmb_add(251);
            }
            if u==252 {
                cmb_add(252);
            }
            if u==253 {
                cmb_add(253);
            }
            if u==254 {
                cmb_add(254);
            }
            if u==255 {
                cmb_add(255);
            }
            if u==256 {
                cmb_add(256);
            }
            if u==257 {
                cmb_add(257);
            }
            if u==258 {
                cmb_add(258);
            }
            if u==259 {
                cmb_add(259);
            }
            
            
        }
        cmb_prompt();
        textView.text = textView.text + inputText + "\n" ;
        
        return inputText
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    

}

