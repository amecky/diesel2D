#include "HudNode.h"
#include "..\..\utils\StringUtils.h"

namespace ds {

// -------------------------------------------------------
// HudNode - Constructor
// -------------------------------------------------------
HudNode::HudNode(Renderer* renderer,int material,int bitmapFont)
	: Node("HudNode",renderer,material) , m_SupportFloatingText(false) {
	m_MeshNode = new SimpleQuadNode("HudQuadNode",renderer,material,800);
	m_TextNode = new TextNode("HudTextNode",renderer,material,bitmapFont);
}

HudNode::~HudNode() {
	delete m_TextNode;
	delete m_MeshNode;
}

uint32 HudNode::addImage(const Vec2& pos,const Rect& rect,float textureSize) {
	return m_MeshNode->create(rect.width(),rect.height(),pos,rect,textureSize);
}
// -------------------------------------------------------
// Adds a static image
// -------------------------------------------------------
uint32 HudNode::addImage(const Vec2& pos,const char* taItemName) {
	return m_MeshNode->create(pos,taItemName);
}

// -------------------------------------------------------
// Adds a new counter
// -------------------------------------------------------
void HudNode::addCounter(const char* name,const Vec2& pos,uint32 digits,const Color& color,float scale /* = 1.0f */) {
	TextCounter* tc = new TextCounter(m_TextNode,pos,digits,color,scale);
	tc->reset();
	CounterEntry entry;
	entry.textCounter = tc;
	entry.hashName = string::murmur_hash(name);
	m_CounterEntries.push_back(entry);
}

// -------------------------------------------------------
// Adds a text
// -------------------------------------------------------
uint32 HudNode::addText(const Vec2& pos,const std::string& text,const Color& color,float scale) {
	return m_TextNode->addText(pos,text,color,scale);
}

void HudNode::updateText(uint32 id,const std::string& txt) {
	m_TextNode->updateText(id,txt);
}
// -------------------------------------------------------
// Calculates the dimension of a given text
// -------------------------------------------------------
Vec2 HudNode::calculateTextSize(const std::string& text,float scale) {
	return m_TextNode->calculateSize(text,scale);
}

// -------------------------------------------------------
// Finds TextCounter for given name
// -------------------------------------------------------
TextCounter* HudNode::findTextCounterByName(const char* counterName) {
	IdString hash = string::murmur_hash(counterName);
	for ( size_t i = 0; i < m_CounterEntries.size(); ++i ) {
		CounterEntry* ce = &m_CounterEntries[i];
		if ( ce->hashName == hash ) {
			return ce->textCounter;
		}
	}
	return 0;
}

// -------------------------------------------------------
// Finds TextCounter for given name - const version
// -------------------------------------------------------
const TextCounter* HudNode::findTextCounterByName(const char* counterName) const {
	IdString hash = string::murmur_hash(counterName);
	for ( size_t i = 0; i < m_CounterEntries.size(); ++i ) {
		const CounterEntry* ce = &m_CounterEntries[i];
		if ( ce->hashName == hash ) {
			return ce->textCounter;
		}
	}
	return 0;
}
// -------------------------------------------------------
// Sets value of a TextCounter
// -------------------------------------------------------
void HudNode::setValue(const char* counterName,int value) {
	TextCounter* tc = findTextCounterByName(counterName);
	if ( tc != 0 ) {
		tc->set(value);
	}	
}

// -------------------------------------------------------
// Add value to a TextCounter
// -------------------------------------------------------
void HudNode::addValue(const char* counterName,int value) {
	TextCounter* tc = findTextCounterByName(counterName);
	if ( tc != 0 ) {
		tc->add(value);
	}	
}
// -------------------------------------------------------
// Gets the value of a specific counter or -1
// -------------------------------------------------------
const int HudNode::getValue(const char* counterName) const {
	const TextCounter* tc = findTextCounterByName(counterName);
	if ( tc != 0 ) {
		return tc->getValue();
	}
	return -1;
}

// -------------------------------------------------------
// Adds a Timer
// -------------------------------------------------------
void HudNode::addTimer(const char* name,const Vec2& pos,const Color& color,float scale) {
	StopWatch* sw = new StopWatch(m_TextNode,pos,color,scale);
	TimerEntry entry;
	entry.hashName = string::murmur_hash(name);
	entry.stopWatch = sw;
	m_TimerEntries.push_back(entry);
}

// -------------------------------------------------------
// Gets minutes from specific timer
// -------------------------------------------------------
const int HudNode::getMinutes(const char* timerName) const {
	IdString hash = string::murmur_hash(timerName);
	for ( size_t i = 0; i < m_TimerEntries.size(); ++i ) {
		const TimerEntry* ce = &m_TimerEntries[i];
		if ( ce->hashName == hash ) {
			return ce->stopWatch->getMinutes();
		}
	}
	return -1;
}

// -------------------------------------------------------
// Gets seconds from specific timer
// -------------------------------------------------------
const int HudNode::getSeconds(const char* timerName) const {
	IdString hash = string::murmur_hash(timerName);
	for ( size_t i = 0; i < m_TimerEntries.size(); ++i ) {
		const TimerEntry* ce = &m_TimerEntries[i];
		if ( ce->hashName == hash ) {
			return ce->stopWatch->getSeconds();
		}
	}
	return -1;
}

// -------------------------------------------------------
// Resets timer
// -------------------------------------------------------
void HudNode::resetTimer(const char* timerName) {
	StopWatch* sw = findByName(timerName);
	if ( sw != 0 ) {
		sw->reset();
	}
}

// -------------------------------------------------------
// Starts timer
// -------------------------------------------------------
void HudNode::startTimer(const char* timerName) {
	StopWatch* sw = findByName(timerName);
	if ( sw != 0 ) {
		sw->start();
	}
}

// -------------------------------------------------------
// Stops timer
// -------------------------------------------------------
void HudNode::stopTimer(const char* timerName) {
	StopWatch* sw = findByName(timerName);
	if ( sw != 0 ) {
		sw->stop();
	}	
}

// -------------------------------------------------------
// Find StopWatch by name - used internally
// -------------------------------------------------------
StopWatch* HudNode::findByName(const char* timerName) {
	IdString hash = string::murmur_hash(timerName);
	for ( size_t i = 0; i < m_TimerEntries.size(); ++i ) {
		TimerEntry* ce = &m_TimerEntries[i];
		if ( ce->hashName == hash ) {
			return ce->stopWatch;
		}
	}
	return 0;
}

// -------------------------------------------------------
// Creates a floating text and enables support of it
// -------------------------------------------------------
void HudNode::createFloatingText() {
	m_FloatingText.textIndex = m_TextNode->addText(Vec2(0,0),"Dummy");
	m_TextNode->setState(m_FloatingText.textIndex,false);
	m_SupportFloatingText = true;
}

// -------------------------------------------------------
// Starts a floating text
// -------------------------------------------------------
void HudNode::startFloatingText(const char* txt,const ds::Vec2& pos,float ttl,float maxScale) {
	m_TextNode->setText(m_FloatingText.textIndex,pos,ds::Color(0.8f,0.8f,0.8f,0.8f),txt);
	m_FloatingText.timer = 0.0f;
	m_FloatingText.floating = true;
	m_FloatingText.ttl = ttl;
	m_FloatingText.pos = pos;
	m_FloatingText.maxScale = maxScale;
	m_FloatingText.size = m_TextNode->getDimension(m_FloatingText.textIndex);
	m_TextNode->setState(m_FloatingText.textIndex,true);
}
// -------------------------------------------------------
// Update
// -------------------------------------------------------
void HudNode::update(float elapsed) {
	for ( size_t i = 0; i < m_CounterEntries.size(); ++i ) {
		const CounterEntry* ce = &m_CounterEntries[i];
		ce->textCounter->update(elapsed);
	}
	for ( size_t i = 0; i < m_TimerEntries.size(); ++i ) {
		const TimerEntry* ce = &m_TimerEntries[i];
		ce->stopWatch->update(elapsed);
	}
	if ( m_SupportFloatingText && m_FloatingText.floating ) {
		m_FloatingText.timer += elapsed;		
		if ( m_FloatingText.timer > m_FloatingText.ttl ) {
			m_FloatingText.floating = false;
			m_TextNode->setColor(m_FloatingText.textIndex,ds::Color(0.8f,0.8f,0.8f,0.0f));

		}
		else {
			float norm = m_FloatingText.timer / m_FloatingText.ttl;
			float a = 1.0f - norm;
			float scale = 1.0f + norm * m_FloatingText.maxScale;
			m_TextNode->setColor(m_FloatingText.textIndex,ds::Color(0.8f,0.8f,0.8f,a));
			m_TextNode->setScale(m_FloatingText.textIndex,scale);
			ds::Vec2 size = m_TextNode->getDimension(m_FloatingText.textIndex);
			float dx = m_FloatingText.pos.x + ( m_FloatingText.size.x - size.x ) * 0.5f;
			float dy = m_FloatingText.pos.y + ( m_FloatingText.size.y - size.y ) * 0.5f;
			m_TextNode->setPosition(m_FloatingText.textIndex,ds::Vec2(dx,dy));
		}
	}
}

// -------------------------------------------------------
// Render both nodes
// -------------------------------------------------------
void HudNode::render() {
	//m_Renderer->drawNode(m_MeshNode);
	m_Renderer->drawNode(m_TextNode);
}

void HudNode::activate() {
	setState(true);
}

void HudNode::deactivate() {
	setState(false);
}

void HudNode::setState(bool active) {
	setActive(active);
	m_MeshNode->setActive(active);
	m_TextNode->setActive(active);
}

}