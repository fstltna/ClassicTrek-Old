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
        // Do here whatever you want with the user's input!
    }
    @IBOutlet var bottomHeight: NSLayoutConstraint!
    
    @IBOutlet var inputField: UITextField!
    
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

        let inputfortextarea: String = ".  .  .  .  .  .  .  . \n.  .  .  .  .  .  .  . \n.  .  *  .  .  .  .  . \n.  .  .  .  *  .  .  . \n"
        // Whatever you pass in the updateTextView Function will be displayed in the textView
        updateTextView(inputfortextarea)
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
    
    override func viewWillDisappear(animated: Bool) {
        NSNotificationCenter.defaultCenter().removeObserver(self, name: UIKeyboardWillShowNotification, object: nil)
        NSNotificationCenter.defaultCenter().removeObserver(self, name: UIKeyboardWillHideNotification, object: nil)
    }

    
    // Whatever passed in the updateTextView Function will be displayed in the textView
    func updateTextView(inputText: String) -> String {
        textView.text = inputText
        return inputText
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

