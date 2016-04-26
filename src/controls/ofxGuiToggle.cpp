#include "ofxGuiToggle.h"
#include "ofxGuiGroup.h"
#include "ofGraphics.h"
using namespace std;

ofxGuiToggle::ofxGuiToggle():ofxGuiElement(){

	setup();

}

ofxGuiToggle::ofxGuiToggle(const string &toggleName):ofxGuiToggle(){

	setName(toggleName);
	value.set(false);

}

ofxGuiToggle::ofxGuiToggle(const string &toggleName, const ofJson& config):ofxGuiToggle(toggleName){

	_setConfig(config);

}

ofxGuiToggle::ofxGuiToggle(ofParameter<bool> &_bVal, const ofJson & config)
	:ofxGuiElement(){

	value.makeReferenceTo(_bVal);
	setup();
	_setConfig(config);

}

ofxGuiToggle::ofxGuiToggle(ofParameter<bool> &_bVal, float width, float height)
	:ofxGuiToggle(_bVal){

	setSize(width, height);

}

ofxGuiToggle::ofxGuiToggle(const std::string& toggleName, bool _bVal, float width, float height)
	:ofxGuiToggle(toggleName){

	value.set(_bVal);
	setSize(width, height);

}

ofxGuiToggle::~ofxGuiToggle(){

	value.removeListener(this,&ofxGuiToggle::valueChanged);

}

void ofxGuiToggle::setup(){

	hasFocus = false;

	type.set("type", ofxGuiToggleType::CHECKBOX);

	setTheme();

	value.addListener(this,&ofxGuiToggle::valueChanged);

	registerMouseEvents();

}

void ofxGuiToggle::_setConfig(const ofJson &config){

	ofxGuiElement::_setConfig(config);

	if (config.find(type.getName()) != config.end()) {
		std::string val = config[type.getName()];
		setType(val);
	}

}

float ofxGuiToggle::getMinWidth(){
	float _width = 0;
	if(showName){
		_width += ofxGuiElement::getTextWidth(getName()) + 2*textPadding;
	}
	if(type != ofxGuiToggleType::FULLSIZE){
		_width += 30;
	}
	return _width;
}

float ofxGuiToggle::getMinHeight(){
	float _height = 0;
	if(showName){
		_height += ofxGuiElement::getTextHeight(getName()) + 2*textPadding;
	}
	return _height;
}

bool ofxGuiToggle::mousePressed(ofMouseEventArgs & args){
	ofxGuiElement::mousePressed(args);
	return setValue(args.x, args.y, true);
}

bool ofxGuiToggle::mouseReleased(ofMouseEventArgs & args){
	ofxGuiElement::mouseReleased(args);
	bool hadFocus = hasFocus;
	hasFocus = false;
	return (hadFocus && isMouseOver());
}

void ofxGuiToggle::generateDraw(){

	switch(type){
		default:
		case ofxGuiToggleType::RADIO:
		case ofxGuiToggleType::CHECKBOX: {
			checkboxRect.set(1, 1, getHeight() - 2, getHeight() - 2);
			break;
		}
		case ofxGuiToggleType::FULLSIZE: {
			checkboxRect.set(1, 1, getWidth() - 2, getHeight() - 2);
			break;
		}
	}

	bg.clear();
	bg.setFilled(true);
	border.clear();
	border.setFillColor(borderColor);
	border.setFilled(true);
	if(value){
		bg.setFillColor(fillColor);
	}else{
		bg.setFillColor(backgroundColor);
	}
	switch(type){
		default:
		case ofxGuiToggleType::RADIO:{
			border.arc(checkboxRect.getCenter(), checkboxRect.getHeight()/3, checkboxRect.getHeight()/3, 0, 360);
			if(value){
				bg.append(border);
			}else{
				bg.arc(checkboxRect.getCenter(), checkboxRect.getHeight()/3-borderWidth, checkboxRect.getHeight()/3-borderWidth, 0, 360);
				border.append(bg);
			}
			break;
		}
		case ofxGuiToggleType::CHECKBOX: {
			border.rectangle(checkboxRect.getTopLeft()+ofPoint(checkboxRect.width/6,checkboxRect.height/6),
						 checkboxRect.width/3*2,checkboxRect.height/3*2);
			if(value){
				bg.append(border);
			}else{
				bg.rectangle(checkboxRect.getTopLeft()+ofPoint(checkboxRect.width/6+borderWidth,checkboxRect.height/6+borderWidth),
							 checkboxRect.width/3*2-2*borderWidth,checkboxRect.height/3*2 - 2*borderWidth);
				border.append(bg);
			}
			break;
		}
		case ofxGuiToggleType::FULLSIZE: {
			border.rectangle(checkboxRect.getTopLeft(),checkboxRect.width,checkboxRect.height);
			if(value){
				bg.append(border);
			}else{
				bg.rectangle(checkboxRect.getTopLeft()+ofPoint(borderWidth,borderWidth),checkboxRect.width-2*borderWidth,checkboxRect.height-2*borderWidth);
				border.append(bg);
			}
			break;
		}
	}

	if(showName){
		switch(type){
			default:
			case ofxGuiToggleType::RADIO:
			case ofxGuiToggleType::CHECKBOX: {

	//			//create cross
	//			cross.clear();
	//			cross.setStrokeColor(textColor);
	//			cross.setStrokeWidth(1);
	//			cross.setFilled(false);
	//			cross.moveTo(checkboxRect.getTopLeft());
	//			cross.lineTo(checkboxRect.getBottomRight());
	//			cross.moveTo(checkboxRect.getTopRight());
	//			cross.lineTo(checkboxRect.getBottomLeft());

				// create label

				textMesh = getTextMesh(getName(), textPadding + checkboxRect.width, getShape().getHeight() / 2 + 4);
				break;
			}
			case ofxGuiToggleType::FULLSIZE: {

				// create label
				float textWidth = ofxGuiElement::getTextWidth(getName());
				switch(textAlignment){
					default:
					case TextAlignment::CENTERED:
						if(getShape().getCenter().x - textWidth/2 > getShape().x+textPadding){
							textMesh = getTextMesh(getName(), getWidth()/2 - textWidth/2, getHeight()/ 2 + 4);
							break;
						}
					case TextAlignment::LEFT:
						textMesh = getTextMesh(getName(), textPadding, getShape().height / 2 + 4);
						break;
					case TextAlignment::RIGHT:
						textMesh = getTextMesh(getName(), getShape().getWidth() - textWidth - textPadding, getShape().height / 2 + 4);
						break;

				}
				break;
			}
		}
	}


}

void ofxGuiToggle::render(){

	border.draw();
	bg.draw();

//	if(value && (type == ofxToggle::Type::CHECKBOX
//			|| type == ofxToggle::Type::RADIO)){
//		cross.draw();
//	}

	if(showName){
		ofColor c = ofGetStyle().color;
		ofBlendMode blendMode = ofGetStyle().blendingMode;
		if(blendMode!=OF_BLENDMODE_ALPHA){
			ofEnableAlphaBlending();
		}
		ofSetColor(textColor);

		bindFontTexture();
		textMesh.draw();
		unbindFontTexture();

		ofSetColor(c);
		if(blendMode!=OF_BLENDMODE_ALPHA){
			ofEnableBlendMode(blendMode);
		}
	}
}

bool ofxGuiToggle::operator=(bool v){
	value = v;
	return v;
}

ofxGuiToggle::operator const bool & (){
	return value;
}

bool ofxGuiToggle::setValue(float mx, float my, bool bCheck){

	if(isHidden()){
		hasFocus = false;
		return false;
	}
	if(bCheck){

		ofRectangle checkRect = checkboxRect;
		checkRect.x += getScreenPosition().x;
		checkRect.y += getScreenPosition().y;

		hasFocus = checkRect.inside(mx, my);
	}

	if(hasFocus){

		//if group has exclusive toggles and toggle is on, don't to anything
		ofxGuiGroup* parent = dynamic_cast<ofxGuiGroup*>(this->parent());
		if(parent){
			if(parent->getTogglesExclusive()){
				if(value.get()){
					return false;
				}
			}
		}

		value = !value;

		if(value.get()){
			if(parent){
				parent->deactivateAllOtherToggles(this);
			}
		}

		setNeedsRedraw();

		return true;
	}

	return false;
}

ofAbstractParameter & ofxGuiToggle::getParameter(){
	return value;
}

void ofxGuiToggle::valueChanged(bool & value){
	setNeedsRedraw();
}

void ofxGuiToggle::setType(const std::string& type){
	if(type == "checkbox"){
		setType(ofxGuiToggleType::CHECKBOX);
	}
	else if(type == "radio"){
		setType(ofxGuiToggleType::RADIO);
	}
	else if(type == "fullsize"){
		setType(ofxGuiToggleType::FULLSIZE);
	}
}

void ofxGuiToggle::setType(const ofxGuiToggleType::Type &type){
	this->type.set(type);
	setNeedsRedraw();
}

ofxGuiToggleType::Type ofxGuiToggle::getType(){
	return type;
}

std::string ofxGuiToggle::getClassType(){
	return "toggle";
}

vector<std::string> ofxGuiToggle::getClassTypes(){
	vector<std::string> types = ofxGuiElement::getClassTypes();
	types.push_back(getClassType());
	return types;
}