
/**
 * First we will load all of this project's JavaScript dependencies which
 * includes Vue and other libraries. It is a great starting point when
 * building robust, powerful web applications using Vue and Laravel.
 */

require('./bootstrap');

window.$ = window.jQuery = require('jquery');
window.Vue = require('vue');

/* Ctrl + S押下時のデフォルト動作を抑制 */
$(function() {
    $(window).keydown(function(e) {
        if ((e.which == '115' || e.which == '83' ) && (e.ctrlKey || e.metaKey)) {
            e.preventDefault();
            return false;
        }
        return true;
    });
});

/**
 * Next, we will create a fresh Vue application instance and attach it to
 * the page. Then, you may begin adding components to this application
 * or customize the JavaScript scaffolding to fit your unique needs.
 */

Vue.component('table-editor', require('./components/table-editor/TableEditorTableComponent.vue'));
Vue.component('table-editor-th', require('./components/table-editor/TableEditorThComponent.vue'));
Vue.component('data-editor-table', require('./components/data-editor/DataEditorTableComponent.vue'));
Vue.component('data-editor-form', require('./components/data-editor/DataEditorFormComponent.vue'));
Vue.component('data-editor-input', require('./components/data-editor/DataEditorInputComponent.vue'));
Vue.component('data-editor-textarea', require('./components/data-editor/DataEditorTextareaComponent.vue'));
Vue.component('data-editor-select', require('./components/data-editor/DataEditorSelectComponent.vue'));

const app = new Vue({
    el: '#app',
    data: {
        tbl_meta: [],
        tbl_data: [],
        selected_template: 0
    },
    methods: {
        setTblMeta(new_meta) {
            this.tbl_meta = new_meta;
        }
    }
});
