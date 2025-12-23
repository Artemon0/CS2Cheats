// Currency switcher
const currencyButtons = document.querySelectorAll('.currency-btn');
const priceAmounts = document.querySelectorAll('.price-amount');
const priceCurrencies = document.querySelectorAll('.price-currency');

const currencySymbols = {
    RUB: '₽',
    UAH: '₴',
    USD: '$'
};

currencyButtons.forEach(button => {
    button.addEventListener('click', () => {
        // Remove active class from all buttons
        currencyButtons.forEach(btn => btn.classList.remove('active'));

        // Add active class to clicked button
        button.classList.add('active');

        // Get selected currency
        const currency = button.dataset.currency;

        // Update prices
        priceAmounts.forEach(amount => {
            const price = amount.dataset[currency.toLowerCase()];
            amount.textContent = price;
        });

        // Update currency symbols
        priceCurrencies.forEach(symbol => {
            symbol.textContent = currencySymbols[currency];
        });
    });
});

// Smooth scrolling
document.querySelectorAll('a[href^="#"]').forEach(anchor => {
    anchor.addEventListener('click', function (e) {
        e.preventDefault();
        const target = document.querySelector(this.getAttribute('href'));
        if (target) {
            target.scrollIntoView({
                behavior: 'smooth',
                block: 'start'
            });
        }
    });
});

// Navbar scroll effect
window.addEventListener('scroll', () => {
    const navbar = document.querySelector('.navbar');
    if (window.scrollY > 50) {
        navbar.style.backgroundColor = 'rgba(44, 47, 51, 0.95)';
    } else {
        navbar.style.backgroundColor = 'var(--secondary-color)';
    }
});

// Animate on scroll
const observerOptions = {
    threshold: 0.1,
    rootMargin: '0px 0px -100px 0px'
};

const observer = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
        if (entry.isIntersecting) {
            entry.target.style.opacity = '1';
            entry.target.style.transform = 'translateY(0)';
        }
    });
}, observerOptions);

// Observe all feature cards and pricing cards
document.querySelectorAll('.feature-card, .pricing-card, .faq-item').forEach(el => {
    el.style.opacity = '0';
    el.style.transform = 'translateY(20px)';
    el.style.transition = 'opacity 0.6s ease, transform 0.6s ease';
    observer.observe(el);
});

// Language switcher
const langButtons = document.querySelectorAll('.lang-btn');
const translatableElements = document.querySelectorAll('[data-lang]');

const setLanguage = async (lang) => {
    // Save language to local storage
    localStorage.setItem('language', lang);

    // Set html lang attribute
    document.documentElement.lang = lang;

    // Load translations
    const response = await fetch(`lang/${lang}.json`);
    const translations = await response.json();

    // Translate elements
    translatableElements.forEach(el => {
        const key = el.dataset.lang;
        if (translations[key]) {
            // Use innerHTML for elements that may contain other tags
            if (key === 'footer_copyright' || key === 'footer_disclaimer' || key.startsWith('faq_q') || el.children.length > 0) {
                el.innerHTML = translations[key];
            } else {
                el.textContent = translations[key];
            }
        }
    });

    // Update active button
    langButtons.forEach(btn => {
        if (btn.dataset.langSwitch === lang) {
            btn.classList.add('active');
        } else {
            btn.classList.remove('active');
        }
    });
};

langButtons.forEach(button => {
    button.addEventListener('click', () => {
        const lang = button.dataset.langSwitch;
        setLanguage(lang);
    });
});

// Set initial language
document.addEventListener('DOMContentLoaded', () => {
    const savedLang = localStorage.getItem('language') || 'ru';
    setLanguage(savedLang);
});